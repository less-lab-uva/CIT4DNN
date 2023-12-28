from torchvision import datasets, transforms
import torch
import torch.utils.data as data
from torchvision.datasets.folder import default_loader
from torchvision import transforms
import matplotlib.pyplot as plt
import os
import numpy as np
import cv2
import matplotlib.image as mpimg
from sklearn.model_selection import train_test_split
from PIL import Image
import time
from tqdm import tqdm
from sklearn.metrics import pairwise_distances
import pandas as pd
import h5py
from network import *

ROW_SIZE = 10

dataset_dims = {"MNIST": (1,28,28), "FashionMNIST": (1,28,28), "TaxiNet": (1,16,32),
                "SVHN": (3,32,32), "CIFAR10": (3,32,32), "Udacity": (3,72,240)}
                
def get_dimensions(dataset):
    return dataset_dims[dataset]
                
def get_input_suffix(dataset):
    return f"{dataset_dims[dataset][1]}x{dataset_dims[dataset][2]}"
    
def create_logdir(vae_arch, dataset, latent_dim, index):
    index_suffix = f"_v{index}" if index else ""
    log_dir = dataset+"_"+vae_arch+"_z"+str(latent_dim)+index_suffix
    os.makedirs('vae_logs/{}'.format(log_dir), exist_ok=True)
    return log_dir, index_suffix
    
def load_vae(config):
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    
    vae_suffix = f"_v{config['default']['vae_sample']}"
    stage_suffix = f"_stage2" if config['default']['vae_arch'] == "TwoStage" else ""
        
    channels, width, height = get_dimensions(config['default']['dataset'])
    input_suffix = get_input_suffix(config['default']['dataset'])
    vae = eval(config['default']['vae_arch']+"VAE")(config['default']['model'], input_suffix, channels, int(config['default']['latent_dim']))
    model_path = os.path.join(os.path.dirname(__file__), f"models/{config['default']['dataset']}_{config['default']['vae_arch']}_z{config['default']['latent_dim']}{stage_suffix}{vae_suffix}.pth")
    vae.load_state_dict(torch.load(model_path, map_location=device))
    return vae
       
#Calculate mean and log variance vectors output by the encoder for the test inputs
def evaluate(model, testloader, twoStage):
    initialize = True
    for data, _ in tqdm(testloader, leave=False):
        if twoStage:
            mean1, _ = model.encoder(data)
            mean, log_var = model.encoder2(mean1)
        else:
            mean, log_var = model.encoder(data)
        if initialize:
            mu = mean
            sd = torch.exp(0.5 * log_var)
            initialize = False
        else:
            mu = torch.cat((mu, mean), 0)
            sd_temp = torch.exp(0.5 * log_var)
            sd = torch.cat((sd, sd_temp), 0)
            
    mu_np = mu.detach()
    sd_np = sd.detach()
    
    print("latent_dist mu_np shape for the mnist test dataset {}".format(mu_np.shape))
    return mu_np, sd_np
    
def odin_mmd(X, Y, kernel='rbf'):
    biased = False
    sigma = np.sqrt(0.5 * np.median(pairwise_distances(X, Y, metric='euclidean')))
    gamma = 1 / (2 * sigma**2)

    XX = np.dot(X, X.T)
    XY = np.dot(X, Y.T)
    YY = np.dot(Y, Y.T)

    X_sqnorms = np.diagonal(XX)
    Y_sqnorms = np.diagonal(YY)

    K_XY = np.exp(-gamma * (
            -2 * XY + X_sqnorms[:, np.newaxis] + Y_sqnorms[np.newaxis, :]))
    K_XX = np.exp(-gamma * (
            -2 * XX + X_sqnorms[:, np.newaxis] + X_sqnorms[np.newaxis, :]))
    K_YY = np.exp(-gamma * (
            -2 * YY + Y_sqnorms[:, np.newaxis] + Y_sqnorms[np.newaxis, :]))

    if biased:
        mmd2 = K_XX.mean() + K_YY.mean() - 2 * K_XY.mean()
    else:
        m = K_XX.shape[0]
        n = K_YY.shape[0]

        mmd2 = (K_XX.sum() / (m * (m - 1))
              + K_YY.sum() / (n * (n - 1))
              - 2 * K_XY.mean())
    return mmd2
    
def MMD3(x, y, kernel='rbf'):
    print("using MMD3 method")
    #sigma2 = (0.5 * np.median(pairwise_distances(x, y, metric='euclidean')))**2
    sigma2 = 0.5 * np.median(pairwise_distances(x, y, metric='euclidean'))
    alpha=1.0/sigma2
    print("alpha ", alpha)
    B = x.size(0)
    xx, yy, zz = torch.mm(x,x.t()), torch.mm(y,y.t()), torch.mm(x,y.t())
    
    rx = (xx.diag().unsqueeze(0).expand_as(xx))
    ry = (yy.diag().unsqueeze(0).expand_as(yy))
    
    K = torch.exp(- 0.5 * alpha * (rx.t() + rx - 2*xx))
    L = torch.exp(- 0.5 * alpha * (ry.t() + ry - 2*yy))
    P = torch.exp(- 0.5 * alpha * (rx.t() + ry - 2*zz))
    
    beta = (1./(B*(B-1)))
    gamma = (2./(B*B)) 

    return beta * (torch.sum(K)+torch.sum(L)) - gamma * torch.sum(P)
        
def generate_and_save_images(test_samples, model, filename="MNIST", vae_arch="VAE", stage=2, color=False, resize=None):
    #assert test_samples.shape[0] >= 100, "insufficient number of test inputs, requires atleast 100"
    row_size = ROW_SIZE
    test_samples = test_samples[:row_size*row_size]
    samples = [test_samples]
    fileNames = [f'./samples/{filename}_{vae_arch}_original.png']
    if model:
        model.eval()
        with torch.no_grad():
            if vae_arch == "TwoStage":
                predictions = model.reconstruct(test_samples, stage=1)
                samples.append(predictions)
                fileNames.append(f'./samples/{filename}_{vae_arch}_generated_stage1.png')
                if stage == 2:
                    predictions = model.reconstruct(test_samples, stage=stage)
                    samples.append(predictions)
                    fileNames.append(f'./samples/{filename}_{vae_arch}_generated_stage2.png')
            else:
                predictions = model.reconstruct(test_samples, stage=3)
                samples.append(predictions)
                fileNames.append(f'./samples/{filename}_{vae_arch}_generated.png')
                
    for i, sample in enumerate(samples):
        if resize:
            t = transforms.Resize(resize)
            sample = t(sample)
        sample = sample.detach().numpy()
        if not color:
            sample = sample.reshape(-1,sample.shape[2],sample.shape[3])
        else:
            print("Sample shape ", sample.shape)
            sample = np.moveaxis(sample,1,-1)
        plot_samples(sample, color, fileNames[i], row_size)
        
def sample_and_save_images(latent_dim, model, filename="MNIST", vae_arch="VAE", color=False, resize=None):
    #assert test_samples.shape[0] >= 100, "insufficient number of test inputs, requires atleast 100"
    row_size = ROW_SIZE
    fileNames = []

    model.eval()
    with torch.no_grad():
        samples = [model.sample(ROW_SIZE*ROW_SIZE, latent_dim)]
        fileNames.append(f'./samples/{filename}_{vae_arch}_sampled.png')
                
    for i, sample in enumerate(samples):
        if resize:
            t = transforms.Resize(resize)
            sample = t(sample)
        sample = sample.detach().numpy()
        if not color:
            sample = sample.reshape(-1,sample.shape[2],sample.shape[3])
        else:
            print("Sample shape ", sample.shape)
            sample = np.moveaxis(sample,1,-1)
        plot_samples(sample, color, fileNames[i], row_size)
    
def plot_samples(samples, color, filename, row_size):
    fig = plt.figure(figsize=(row_size, row_size))
    for i in range(row_size*row_size):
        plt.subplot(row_size, row_size, i + 1)
        #print(samples[i,0,0])
        #print("DEBUG plot_samples ", samples.shape)
        sample = (samples[i]*255).astype('uint8')
        #print(sample[0,0])
        #print(samples[i].shape, sample.shape)
        
        if not color:
            plt.imshow(samples[i], cmap='gray')
        else:
            plt.imshow(sample)
            fname = filename.replace(".png", f"_{i}.png")
            mpimg.imsave(fname, sample)
        plt.axis('off')
        plt.xticks([])
        plt.yticks([])
        plt.tight_layout()
    plt.savefig(filename)
 
#28x28 
def get_MNIST_dataloader(batch_size, train=True, shuffle=True):
    mnist_transforms = transforms.Compose([transforms.ToTensor()])
    data_loader = torch.utils.data.DataLoader(datasets.MNIST('../data', train=train, download=True, transform=mnist_transforms), batch_size=batch_size, shuffle=shuffle)
    return data_loader

#28x28 
def get_FashionMNIST_dataloader(batch_size, train=True, shuffle=True):
    fmnist_transforms = transforms.Compose([transforms.ToTensor()])
    data_loader = torch.utils.data.DataLoader(datasets.FashionMNIST('../data', train=train, download=True, transform=fmnist_transforms), batch_size=batch_size, shuffle=shuffle)
    return data_loader

#32x32
def get_CIFAR10_dataloader(batch_size, train=True, shuffle=True):
    cifar10_transforms = transforms.Compose([transforms.ToTensor()])
    data_loader = torch.utils.data.DataLoader(datasets.CIFAR10('../data', train=train, download=True, transform=cifar10_transforms), batch_size=batch_size, shuffle=shuffle)
    return data_loader
    
#3x32x32
def get_SVHN_dataloader(batch_size, train=True, shuffle=True):
    svhn_transforms = transforms.Compose([transforms.ToTensor()])
    if train:
        arg_split = "train"
    else:
        arg_split = "test"
    data_loader = torch.utils.data.DataLoader(datasets.SVHN('../data/SVHN', split=arg_split, download=True, transform=svhn_transforms), batch_size=batch_size, shuffle=shuffle)
    return data_loader

#--------------------------Udacity start---------------------------------------------------------------
def get_Udacity_dataloader(batch_size, data_dir="../data/Udacity", train=True, shuffle=True):
    udacity_transforms = transforms.Compose([transforms.ToPILImage(),transforms.Resize((dataset_dims["Udacity"][1],dataset_dims["Udacity"][2])),transforms.ToTensor()]) #,
    #gen_Udacity_numpy(data_dir, test_fraction=0.2)

    x_train, x_test, y_train, y_test = load_Udacity_data(data_dir)
    
    print("In getUdacity dataloader ", x_train.shape, x_test.shape)
    if train:
        dataset = UdacityDataset(x_train, y_train, batch_size, data_dir, transform=udacity_transforms)
    else:
        dataset = UdacityDataset(x_test, y_test, batch_size, data_dir, transform=udacity_transforms)
    data_loader = torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=shuffle)
    return data_loader
          
          
def load_image(data_dir, image_file):
    """
    Load RGB images from a file
    """
    image_dir = os.path.join(data_dir)
    local_path = "/".join(image_file.split("/")[-4:-1]) + "/" + image_file.split("/")[-1]
    img_path = "{0}/{1}".format(image_dir, local_path)
    
    try:
        return mpimg.imread(img_path) #RGB
    except FileNotFoundError:
        print("File not found: %s" % image_file)
        exit()

   
class UdacityDataset(data.Dataset):
    def __init__(self, images, steering_angles, batch_size, data_dir, transform=None):
        self.batch_size = batch_size
        self.images = images
        self.steering_angles = steering_angles
        self.data_dir = data_dir
        self.transform = transform
        
    def __getitem__(self, index):
        steering_angle = self.steering_angles[index]
        image = self.images[index]  #totensor normalization only works for PIL images
        image = self.transform(image)
        return image, steering_angle

    def __len__(self):
        return len(self.images) // self.batch_size
    
def load_Udacity_data(data_dir="../data/Udacity"):
    suffix = "_75"
    x_train_imgs = np.load(os.path.join(data_dir,f"x_train{suffix}.npy")).astype('uint8')
    y_train = np.load(os.path.join(data_dir,f"y_train{suffix}.npy"))
    x_valid_imgs = np.load(os.path.join(data_dir,f"x_valid{suffix}.npy")).astype('uint8')
    y_valid = np.load(os.path.join(data_dir,f"y_valid{suffix}.npy"))
    print("Loading data complete ",x_train_imgs.shape, " ", x_valid_imgs.shape)
    return x_train_imgs, x_valid_imgs, y_train, y_valid
    
def gen_Udacity_numpy(data_dir="../data/Udacity", test_fraction=0.2):
    tracks = ["track1"]
    drive = ['normal', 'recovery', 'reverse']
    start = time.time()
    x = None
    y = None
    path = None
    x_train = None
    y_train = None
    x_valid = None
    y_valid = None

    for track in tracks:
        for drive_style in drive:
            try:
                path = os.path.join(data_dir, track, drive_style, 'driving_log.csv')
                data_df = pd.read_csv(path)
                if x is None:
                    x = data_df[['center', 'left', 'right']].values
                    y = data_df['steering'].values
                else:
                    x = np.concatenate((x, data_df[['center', 'left', 'right']].values), axis=0)
                    y = np.concatenate((y, data_df['steering'].values), axis=0)
            except FileNotFoundError:
                print("Unable to read file %s" % path)
                continue

    if x is None or y is None:
        print("No driving data were provided for training. Provide correct paths to the driving_log.csv files")
        exit()

    try:
        x_train, x_valid, y_train, y_valid = train_test_split(x, y, test_size=test_fraction, random_state=0)
    except TypeError:
        print("Missing header to csv files")
        exit()

    print("Train dataset: " + str(len(x_train)) + " elements")
    print("Test dataset: " + str(len(x_valid)) + " elements")
    
    x_train_imgs = np.empty((0,75,320,3), dtype=np.single)
    for index in range(x_train.shape[0]):   #
        center, left, right = x_train[index]
        img = np.array(load_image(data_dir, center)).astype(np.single)
        #original image shape 160,320,3
        img = img[60:-25, :, :]
        img = img.reshape(1,75,320,3)
        x_train_imgs = np.concatenate((x_train_imgs, img), axis=0)
    np.save(os.path.join(data_dir,"x_train_75.npy"), x_train_imgs)
    np.save(os.path.join(data_dir,"y_train_75.npy"), y_train)
    
    x_valid_imgs = np.empty((0,75,320,3), dtype=np.single)
    for index in range(x_valid.shape[0]):  #
        center, left, right = x_valid[index]
        img = np.array(load_image(data_dir, center)).astype(np.single)
        img = img[60:-25, :, :] 
        img = img.reshape(1,75,320,3)
        x_valid_imgs = np.concatenate((x_valid_imgs, img), axis=0)
    print("Data processing complete")
    print("Time Taken for data processing is ", time.time()-start, "secs")
    print("dataset size ", x_train_imgs.shape)
    np.save(os.path.join(data_dir,"x_valid_75.npy"), x_valid_imgs)
    np.save(os.path.join(data_dir,"y_valid_75.npy"), y_valid)
#-------------------------------TaxiNet end-------------------------------------------------------------------
def get_TaxiNet_dataloader(batch_size, data_dir="../data/TaxiNet", train=True, shuffle=True):
    if train:
        partition = "train"
    else:
        partition = "test"
    TaxiNet_transforms = transforms.Compose([transforms.ToPILImage(), transforms.ToTensor()])
    #gen_TaxiNet_numpy(data_dir, test_fraction=0.2)
    x_train, x_test, y_train, y_test = load_TaxiNet_data(data_dir+"/32x16.h5")
    print("In get TaxiNet dataloader ", x_train.shape, x_test.shape)
    print(f"Range: {np.min(x_train)} to {np.max(x_train)}")
    dataset = TaxiNetDataset(data_dir+"/32x16.h5", partition, TaxiNet_transforms)
    data_loader = torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=shuffle)
    return data_loader
    
class TaxiNetDataset(data.Dataset):
    def __init__(self, path, partition, transform):
        super(TaxiNetDataset, self).__init__()
        dataset = h5py.File(path, "r")
        self.transform = transform
        if partition == "train":
            self.X = torch.from_numpy(np.array(dataset["X_train"], dtype=np.float32))
            self.Y = torch.from_numpy(np.array(dataset["y_train"], dtype=np.float32))
        elif partition == "test":
            self.X = torch.from_numpy(np.array(dataset["X_val"], dtype=np.float32))
            self.Y = torch.from_numpy(np.array(dataset["y_val"], dtype=np.float32))
        else:
            assert False
        self.X = self.X.view(-1, 1, 16, 32)

    def __len__(self):
        # print(self.X.shape, self.Y.shape)
        assert len(self.X) == len(self.Y)
        return len(self.X)

    def __getitem__(self, idx):
        return self.transform(self.X[idx]), self.Y[idx]

def load_TaxiNet_data(path):
    training_data_file = h5py.File(path, "r")
    X_train = np.array(training_data_file["X_train"])
    Y_train = np.array(training_data_file["y_train"])
    X_test = np.array(training_data_file["X_val"])
    Y_test = np.array(training_data_file["y_val"])
    return X_train, X_test, Y_train, Y_test 
#-------------------------------TaxiNet end-------------------------------------------------------------------