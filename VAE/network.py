import torch
import torch.nn as nn
import numpy as np
from torch.nn.utils import spectral_norm
import torch.nn.functional as F

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

def softclip(tensor, min):
    """ Clips the tensor values at the minimum value min in a softway. Taken from Handful of Trials """
    result_tensor = min + F.softplus(tensor - min)

    return result_tensor

class VAE(nn.Module):
    def __init__(self, model_type, input_size, channels, latent_dim):
        super(VAE, self).__init__()
        self.device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
        self.encoder = eval("Encoder_"+input_size)(channels, latent_dim)
        self.decoder = eval("Decoder_"+input_size)(channels, latent_dim)
        self.latent_dim = latent_dim
        self.channels = channels
        self.model = model_type
        self.log_sigma = 0
        
        if self.model == 'sigma_vae':
            ## Sigma VAE
            self.log_sigma = torch.nn.Parameter(torch.full((1,), 0, dtype=torch.float32)[0], requires_grad=self.model == 'sigma_vae')

    def reparameterize(self, mean, logvar):
        eps = torch.randn_like(mean)
        return mean + torch.exp(logvar / 2) * eps
        
    def reconstruction_loss(self, x_hat, x):
        """ Computes the likelihood of the data given the latent variable,
        in this case using a Gaussian distribution with mean predicted by the neural network and variance = 1 """
        
        if self.model == 'gaussian_vae':
            # Naive gaussian VAE uses a constant variance
            log_sigma = torch.zeros([], device=x_hat.device)
        elif self.model == 'sigma_vae':
            # Sigma VAE learns the variance of the decoder as another parameter
            log_sigma = self.log_sigma
        elif self.model == 'optimal_sigma_vae':
            
            log_sigma = ((x - x_hat) ** 2).mean([0,1,2,3], keepdim=True).sqrt().log()
            self.log_sigma = log_sigma.item()
        else:
            raise NotImplementedError

        # Learning the variance can become unstable in some cases. Softly limiting log_sigma to a minimum of -6
        # ensures stable training.
        log_sigma = softclip(log_sigma, -6)
        
        rec = gaussian_nll(x_hat, log_sigma, x).sum()
    
        return rec
        
    def forward(self, x, stage=1):
        raise NotImplementedError("Please Implement this method")
        
        
    def reconstruct(self, x, stage=1):
        raise NotImplementedError("Please Implement this method")
        
    def sample(self, sampleSize):
        raise NotImplementedError("Please Implement this method")

    def loss_function(self, recon_x, x, mu, logvar, x_decoder=None, stage=None):
        raise NotImplementedError("Please Implement this method")

class KingmaVAE(VAE):
    def __init__(self, model_type, input_size, channels, latent_dim):
        super().__init__(model_type, input_size, channels, latent_dim)
        
    def forward(self, x, stage=1):
        mean, log_var = self.encoder(x)
        z = self.reparameterize(mean, log_var)
        x_hat = self.decoder(z)
        return x_hat, mean, log_var
        
    def reconstruct(self, x, stage=1):
        mean, log_var = self.encoder(x)
        #z = self.reparameterize(mean, log_var)
        x_hat = self.decoder(mean)
        return x_hat
        
    def sample(self, sampleSize):
        z = torch.randn((sampleSize, self.latent_dim)).to(self.device)
        x_hat = self.decoder(z)
        return x_hat

    def loss_function(self, recon_x, x, mu, logvar):
        # Important: both reconstruction and KL divergence loss have to be summed over all element!
        # Here we also sum the over batch and divide by the number of elements in the data later
        if self.model == 'mse_vae':
            rec = torch.nn.MSELoss()(recon_x, x)
        else:
            rec = self.reconstruction_loss(recon_x, x)
        
        # see Appendix B from VAE paper:
        # Kingma and Welling. Auto-Encoding Variational Bayes. ICLR, 2014
        # https://arxiv.org/abs/1312.6114
        # 0.5 * sum(1 + log(sigma^2) - mu^2 - sigma^2)
        kl = -0.5 * torch.sum(1 + logvar - mu.pow(2) - logvar.exp())
        
        return rec, kl
        
    def ue_loss_function(self, recon_x, x, mu, logvar):
        # Important: both reconstruction and KL divergence loss have to be summed over all element!
        # Here we also sum the over batch and divide by the number of elements in the data later
        if self.model == 'mse_vae':
            rec = torch.nn.MSELoss()(recon_x, x)
        else:
            rec = self.reconstruction_loss(recon_x, x)
        
        # see Appendix B from VAE paper:
        # Kingma and Welling. Auto-Encoding Variational Bayes. ICLR, 2014
        # https://arxiv.org/abs/1312.6114
        # 0.5 * sum(1 + log(sigma^2) - mu^2 - sigma^2)
        kl = -0.5 * torch.sum(1 + logvar - mu.pow(2) - logvar.exp())
        
        return rec, kl


def gaussian_nll(mu, log_sigma, x):
    return 0.5 * torch.pow((x - mu) / log_sigma.exp(), 2) + log_sigma + 0.5 * np.log(2 * np.pi)



def weights_init(m):
    classname = m.__class__.__name__
    if classname.find('Conv') != -1:
        nn.init.normal_(m.weight.data, 0.0, 0.02)
    elif classname.find('BatchNorm') != -1:
        nn.init.normal_(m.weight.data, 1.0, 0.02)
        nn.init.constant_(m.bias.data, 0)
        
        
class TwoStageVAE(VAE):
    def __init__(self, model_type, input_size, channels, latent_dim, depth_stage2=3, hidden_dim_stage2=1024):
        super().__init__(model_type, input_size, channels, latent_dim)
        self.encoder2 = Encoder_Stage2(latent_dim, depth_stage2, hidden_dim_stage2)
        self.decoder2 = Decoder_Stage2(latent_dim, depth_stage2, hidden_dim_stage2)
        self.log_sigma2 = 0
        
        if self.model == 'sigma_vae':
            ## Sigma VAE
            self.log_sigma2 = torch.nn.Parameter(torch.full((1,), 0, dtype=torch.float32)[0], requires_grad=self.model == 'sigma_vae')
        
    def forward(self, x, stage=1):
        if stage == 1:
            mean, log_var = self.encoder(x)
            z = self.reparameterize(mean, log_var)
            x_hat = self.decoder(z)
            return x_hat, mean, log_var
        else:
            mean, log_var = self.encoder2(x)
            z = self.reparameterize(mean, log_var)
            x_hat = self.decoder2(z)
            return x_hat, mean, log_var
            
    def reencode(self, z, stage=2):
        if stage == 1:
            x_hat = self.decoder(z)
            mean, log_var = self.encoder(x_hat)
        else:
            z_hat = self.decoder2(z)
            x_hat = self.decoder(z_hat)
            mean_stage1, log_var_stage1 = self.encoder(x_hat)
            mean, log_var = self.encoder2(mean_stage1)
        return mean, log_var
            
        
    def loss_function(self, recon_x, x, mu, logvar, stage):
        # Important: both reconstruction and KL divergence loss have to be summed over all element!
        # Here we also sum the over batch and divide by the number of elements in the data later
        if stage == 1:
            if self.model == 'optimal_sigma_vae':
                log_sigma = ((x - recon_x) ** 2).mean([0,1,2,3], keepdim=True).sqrt().log()
                self.log_sigma = log_sigma.item()  
            else:
                # Sigma VAE learns the variance of the decoder as another parameter
                log_sigma = self.log_sigma
        else:
            if self.model == 'optimal_sigma_vae':
                log_sigma = ((x - recon_x) ** 2).mean([0,1], keepdim=True).sqrt().log()
                self.log_sigma2 = log_sigma.item()  
            else:
                # Sigma VAE learns the variance of the decoder as another parameter
                log_sigma = self.log_sigma2
        log_sigma = softclip(log_sigma, -6)
        rec = gaussian_nll(recon_x, log_sigma, x).sum()
        # see Appendix B from VAE paper:
        # Kingma and Welling. Auto-Encoding Variational Bayes. ICLR, 2014
        # https://arxiv.org/abs/1312.6114
        # 0.5 * sum(1 + log(sigma^2) - mu^2 - sigma^2)
        kl = -0.5 * torch.sum(1 + logvar - mu.pow(2) - logvar.exp())
        
        return rec, kl
        
    def reconstruct(self, x, stage=2):
        if stage == 1:
            mean, log_var = self.encoder(x)
            #z = self.reparameterize(mean, log_var)
            x_hat = self.decoder(mean)
        else:
            mean_stage1, log_var_stage1 = self.encoder(x)
            #print(mean_stage1)
            mean_stage2, log_var_stage2 = self.encoder2(mean_stage1)
            #print(mean_stage2)
            #z = self.reparameterize(mean_stage2, log_var_stage2)
            z_hat = self.decoder2(mean_stage2)
            #print(z_hat)
            x_hat = self.decoder(z_hat)
        return x_hat
        
    def sample(self, sampleSize, stage=2):
        z = torch.randn((sampleSize, self.latent_dim)).to(self.device)
        if stage == 1:
            x_hat = self.decoder(z)
        else:
            z_hat = self.decoder2(z)
            x_hat = self.decoder(z_hat)
        return x_hat

        
class Encoder_Stage2(nn.Module):
    def __init__(self, latent_dim, depth_stage2, hidden_dim):
        super(Encoder_Stage2, self).__init__()
        self.depth_stage2 = depth_stage2
        self.latent_dim = latent_dim
        
        # Fully connected layers
        self.lin1 = nn.Linear(latent_dim, hidden_dim)
        self.lin2 = nn.Linear(hidden_dim, hidden_dim)
        # Fully connected layers for mean and variance
        self.mu_logvar_gen = nn.Linear(hidden_dim+self.latent_dim, self.latent_dim * 2)
        
    def forward(self, t):
        x = torch.relu(self.lin1(t))
        for i in range(self.depth_stage2-1):
            x = torch.relu(self.lin2(x))
            
        x = torch.cat((t, x), dim=-1)
        # Fully connected layer for log variance and mean
        mu_logvar = self.mu_logvar_gen(x)
        mu, logvar = mu_logvar.view(-1, self.latent_dim, 2).unbind(-1)
        return mu, logvar
        
class Decoder_Stage2(nn.Module):
    def __init__(self, latent_dim, depth_stage2, hidden_dim):
        super(Decoder_Stage2, self).__init__()
        # Layer parameters
        self.depth_stage2 = depth_stage2
        
        # Fully connected layers
        self.lin1 = nn.Linear(latent_dim, hidden_dim)
        self.lin2 = nn.Linear(hidden_dim, hidden_dim)
        self.lin3 = nn.Linear(hidden_dim+latent_dim, latent_dim)

    def forward(self, z):
        x = torch.relu(self.lin1(z))
        for i in range(self.depth_stage2-1):
            x = torch.relu(self.lin2(x))
        x = torch.cat((z, x), dim=-1)
        x = self.lin3(x)
        return x
                
#MNIST Encoder architecture from https://arxiv.org/pdf/1804.03599.pdf%20
class Encoder_28x28(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Encoder_28x28, self).__init__()
        self.latent_dim = latent_dim
        hid_channels = 32
        kernel_size = 4
        hidden_dim = 256
        self.reshape = (hid_channels, 7, 7)
        cnn_kwargs = dict(stride=2, padding=1)
        
        self.conv1 = nn.Conv2d(channels, hid_channels, kernel_size, **cnn_kwargs)
        self.conv2 = nn.Conv2d(hid_channels, hid_channels, kernel_size, **cnn_kwargs)
        
        self.lin1 = nn.Linear(np.product(self.reshape), hidden_dim)
        self.lin2 = nn.Linear(hidden_dim, hidden_dim)
        
        self.mu_gen = nn.Linear(hidden_dim, self.latent_dim)
        self.logvar_gen = nn.Linear(hidden_dim, self.latent_dim)
        
    def forward(self, x):
        torch.autograd.set_detect_anomaly(True)
        batch_size = x.size(0)
        x = self.conv1(x)
        x = torch.relu(x)
        x = self.conv2(x)
        x = torch.relu(x)
        x = x.view((batch_size, -1))
        x = self.lin1(x)
        x = torch.relu(x)
        x = self.lin2(x)
        x = torch.relu(x)
        mu = self.mu_gen(x)
        logvar = self.logvar_gen(x)
        return mu, logvar
        
class Decoder_28x28(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Decoder_28x28, self).__init__()
        hid_channels = 32
        kernel_size = 4
        hidden_dim = 256
     
        self.reshape = (hid_channels, 7, 7)
        cnn_kwargs = dict(stride=2, padding=1)
        
        self.lin1 = nn.Linear(latent_dim, hidden_dim)
        self.lin2 = nn.Linear(hidden_dim, hidden_dim)
        self.lin3 = nn.Linear(hidden_dim, np.product(self.reshape))
        
        self.convT1 = nn.ConvTranspose2d(hid_channels, hid_channels, kernel_size, **cnn_kwargs)
        self.convT2 = nn.ConvTranspose2d(hid_channels, channels, kernel_size, **cnn_kwargs)

    def forward(self, z):
        batch_size = z.size(0)
        x = self.lin1(z)
        x = torch.relu(x)
        x = self.lin2(x)
        x = torch.relu(x)
        x = self.lin3(x)
        x = torch.relu(x)
        x = x.view(batch_size, *self.reshape)
        x = self.convT1(x)
        x = torch.relu(x)
        x = self.convT2(x)
        x = torch.sigmoid(x)
        return x

#InfoGAN from https://arxiv.org/pdf/1606.03657.pdf
#Architecture used for color 32x32 images 
#Modified InfoGAN for 16x32
#TaxiNet
class Encoder_16x32(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Encoder_16x32, self).__init__()
        self.latent_dim = latent_dim
        kernel_size = 4
        self.reshape = (256, 2, 4)
        
        cnn_kwargs = dict(stride=2, padding=1)
        self.conv1 = nn.Conv2d(channels, 64, kernel_size, **cnn_kwargs)
        self.conv2 = nn.Conv2d(64, 128, kernel_size, **cnn_kwargs)
        self.conv3 = nn.Conv2d(128, 256, kernel_size, **cnn_kwargs)
        self.lin1 = nn.Linear(np.product(self.reshape), 128)
        
        self.conv1_bn = nn.BatchNorm2d(64)
        self.conv2_bn = nn.BatchNorm2d(128)
        self.conv3_bn = nn.BatchNorm2d(256)
        self.lin1_bn = nn.BatchNorm1d(128)
        
        self.mu_gen = nn.Linear(128, self.latent_dim)
        self.logvar_gen = nn.Linear(128, self.latent_dim)
        
    def forward(self, x):
        #torch.autograd.set_detect_anomaly(True)
        batch_size = x.size(0)
        x = self.conv1(x)
        x = self.conv1_bn(x)
        x = torch.relu(x)
        x = self.conv2(x)
        x = self.conv2_bn(x)
        x = torch.relu(x)
        x = self.conv3(x)
        x = self.conv3_bn(x)
        x = torch.relu(x)
        x = x.view((batch_size, -1))
        x = self.lin1(x)
        x = self.lin1_bn(x)
        x = torch.relu(x)
        mu = self.mu_gen(x)
        logvar = self.logvar_gen(x)
        return mu, logvar

class Decoder_16x32(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Decoder_16x32, self).__init__()
        self.latent_dim = latent_dim
        kernel_size = 4
        self.reshape = (256, 2, 4)
        cnn_kwargs = dict(stride=2, padding=1)
        
        self.lin1 = nn.Linear(latent_dim, 128)
        self.lin2 = nn.Linear(128, np.product(self.reshape))
        self.convT1 = nn.ConvTranspose2d(256, 128, kernel_size, **cnn_kwargs)
        self.convT2 = nn.ConvTranspose2d(128, 64, kernel_size, **cnn_kwargs)
        self.convT3 = nn.ConvTranspose2d(64, channels, kernel_size, **cnn_kwargs)
        
        self.convT1_bn = nn.BatchNorm2d(128)
        self.convT2_bn = nn.BatchNorm2d(64)
        self.lin1_bn = nn.BatchNorm1d(128)
        self.lin2_bn = nn.BatchNorm1d(np.product(self.reshape))

    def forward(self, z):
        batch_size = z.size(0)
        x = self.lin1(z)
        x = self.lin1_bn(x)
        x = torch.relu(x)
        x = self.lin2(x)
        x = self.lin2_bn(x)
        x = torch.relu(x)
        x = x.view(batch_size, *self.reshape)
        x = self.convT1(x)
        x = self.convT1_bn(x)
        x = torch.relu(x)
        x = self.convT2(x)
        x = self.convT2_bn(x)
        x = torch.relu(x)
        x = self.convT3(x)
        x = torch.sigmoid(x)
        return x

        
#InfoGAN from https://arxiv.org/pdf/1606.03657.pdf
#Architecture used for color 32x32 images
class Encoder_32x32(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Encoder_32x32, self).__init__()
        self.latent_dim = latent_dim
        kernel_size = 4
        self.reshape = (256, 4, 4)
        cnn_kwargs = dict(stride=2, padding=1)
        self.conv1 = nn.Conv2d(channels, 64, kernel_size, **cnn_kwargs)
        self.conv2 = nn.Conv2d(64, 128, kernel_size, **cnn_kwargs)
        self.conv3 = nn.Conv2d(128, 256, kernel_size, **cnn_kwargs)
        self.lin1 = nn.Linear(np.product(self.reshape), 128)
        
        self.conv1_bn = nn.BatchNorm2d(64)
        self.conv2_bn = nn.BatchNorm2d(128)
        self.conv3_bn = nn.BatchNorm2d(256)
        self.lin1_bn = nn.BatchNorm1d(128)
        
        self.mu_gen = nn.Linear(128, self.latent_dim)
        self.logvar_gen = nn.Linear(128, self.latent_dim)
        
    def forward(self, x):
        torch.autograd.set_detect_anomaly(True)
        batch_size = x.size(0)
        x = self.conv1(x)
        x = self.conv1_bn(x)
        x = torch.relu(x)
        x = self.conv2(x)
        x = self.conv2_bn(x)
        x = torch.relu(x)
        x = self.conv3(x)
        x = self.conv3_bn(x)
        x = torch.relu(x)
        x = x.view((batch_size, -1))
        x = self.lin1(x)
        x = self.lin1_bn(x)
        x = torch.relu(x)
        mu = self.mu_gen(x)
        logvar = self.logvar_gen(x)
        return mu, logvar

class Decoder_32x32(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Decoder_32x32, self).__init__()
        self.latent_dim = latent_dim
        kernel_size = 4
        self.reshape = (256, 4, 4)
        cnn_kwargs = dict(stride=2, padding=1)
        
        self.lin1 = nn.Linear(latent_dim, 128)
        self.lin2 = nn.Linear(128, np.product(self.reshape))
        self.convT1 = nn.ConvTranspose2d(256, 128, kernel_size, **cnn_kwargs)
        self.convT2 = nn.ConvTranspose2d(128, 64, kernel_size, **cnn_kwargs)
        self.convT3 = nn.ConvTranspose2d(64, channels, kernel_size, **cnn_kwargs)
        
        self.convT1_bn = nn.BatchNorm2d(128)
        self.convT2_bn = nn.BatchNorm2d(64)
        self.lin1_bn = nn.BatchNorm1d(128)
        self.lin2_bn = nn.BatchNorm1d(np.product(self.reshape))

    def forward(self, z):
        batch_size = z.size(0)
        x = self.lin1(z)
        x = self.lin1_bn(x)
        x = torch.relu(x)
        x = self.lin2(x)
        x = self.lin2_bn(x)
        x = torch.relu(x)
        x = x.view(batch_size, *self.reshape)
        x = self.convT1(x)
        x = self.convT1_bn(x)
        x = torch.relu(x)
        x = self.convT2(x)
        x = self.convT2_bn(x)
        x = torch.relu(x)
        x = self.convT3(x)
        x = torch.sigmoid(x)
        return x

#Udacity
#Modified InfoGAN
class Encoder_72x240(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Encoder_72x240, self).__init__()
        self.latent_dim = latent_dim
        kernel_size = 4

        #self.reshape = (256, 9, 20) #after cropping image #for 160
        self.reshape = (256, 9, 30)
        # Convolutional layers
        cnn_kwargs = dict(stride=2, padding=1)
        self.conv1 = nn.Conv2d(channels, 64, kernel_size, **cnn_kwargs)
        self.conv1_bn = nn.BatchNorm2d(64)
        self.conv2 = nn.Conv2d(64, 128, kernel_size, **cnn_kwargs)
        self.conv2_bn = nn.BatchNorm2d(128)
        self.conv3 = nn.Conv2d(128, 256, kernel_size, **cnn_kwargs)
        self.conv3_bn = nn.BatchNorm2d(256)
        
        # Fully connected layers
        self.lin1 = nn.Linear(np.product(self.reshape), 10240)  #23552 for 0.75 factor
        self.lin1_bn = nn.BatchNorm1d(10240)
        self.lin2 = nn.Linear(10240, 4028)
        self.lin2_bn = nn.BatchNorm1d(4028)
        self.lin3 = nn.Linear(4028, 1024)
        self.lin3_bn = nn.BatchNorm1d(1024)
        
        # Fully connected layers for mean and variance
        self.mu_logvar_gen = nn.Linear(1024, self.latent_dim * 2)
        
    def forward(self, x):
        batch_size = x.size(0)
        x = self.conv1(x)
        x = self.conv1_bn(x)
        x = torch.relu(x)
        x = self.conv2(x)
        x = self.conv2_bn(x)
        x = torch.relu(x)
        x = self.conv3(x)
        x = self.conv3_bn(x)
        x = torch.relu(x)
        x = x.view((batch_size, -1))
        x = self.lin1(x)
        x = self.lin1_bn(x)
        x = torch.relu(x)
        x = self.lin2(x)
        x = self.lin2_bn(x)
        x = torch.relu(x)
        x = self.lin3(x)
        x = self.lin3_bn(x)
        x = torch.relu(x)
        
        mu_logvar = self.mu_logvar_gen(x)
        mu, logvar = mu_logvar.view(-1, self.latent_dim, 2).unbind(-1)

        return mu, logvar
        
class Decoder_72x240(nn.Module):
    def __init__(self, channels, latent_dim):
        super(Decoder_72x240, self).__init__()
        kernel_size = 4
        self.reshape = (256, 9, 30)
        # Fully connected layers
        self.lin1 = nn.Linear(latent_dim, 1024)
        self.lin1_bn = nn.BatchNorm1d(1024)
        self.lin2 = nn.Linear(1024, 4028)
        self.lin2_bn = nn.BatchNorm1d(4028)
        self.lin3 = nn.Linear(4028, 10240)
        self.lin3_bn = nn.BatchNorm1d(10240)
        self.lin4 = nn.Linear(10240, np.product(self.reshape))
        self.lin4_bn = nn.BatchNorm1d(np.product(self.reshape))
        
        # Convolutional layers
        cnn_kwargs = dict(stride=2, padding=1)
        self.convT2 = nn.ConvTranspose2d(256, 128, kernel_size, **cnn_kwargs)
        self.convT2_bn = nn.BatchNorm2d(128)
        self.convT3 = nn.ConvTranspose2d(128, 64, kernel_size, **cnn_kwargs)
        self.convT3_bn = nn.BatchNorm2d(64)
        self.convT4 = nn.ConvTranspose2d(64, 3, kernel_size, **cnn_kwargs)
        
    def forward(self, x):
        batch_size = x.size(0)
        #print("DEBUG ", x.size())
        x = self.lin1_bn(self.lin1(x))
        x = torch.relu(x)
        x = self.lin2_bn(self.lin2(x))
        x = torch.relu(x)
        x = self.lin3_bn(self.lin3(x))
        x = torch.relu(x)
        x = self.lin4_bn(self.lin4(x))
        x = torch.relu(x)
        x = x.view(batch_size, *self.reshape)
        x = self.convT2_bn(self.convT2(x))
        x = torch.relu(x)
        x = self.convT3_bn(self.convT3(x))
        x = torch.relu(x)
        x = torch.sigmoid(self.convT4(x))
        return x