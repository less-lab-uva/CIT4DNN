import sys
sys.path.append("./VAE")
import os
import numpy as np
from VAE.VAE_utils import *
from utils import *
from torchvision.utils import save_image
from torch.utils.data import TensorDataset, DataLoader
import argparse
import configparser
import random
from torchvision import transforms
import time

if __name__ == "__main__":
    start = time.time()
    parser = argparse.ArgumentParser()
    parser.add_argument('--dataset', type=str, default='MNIST', choices=["MNIST", "SVHN", "FashionMNIST", "TaxiNet", "Udacity"])
    parser.add_argument('--strategy', type=str, default='density', choices=["density", "mean", "max_density", "random"])
    parser.add_argument('--factor', type=int, default=1, help="Latent sample factor")
    parser.add_argument("--density_range", type=str, default=None)
    parser.add_argument("--ways", type=int, default=None)
    parser.add_argument("--partitions", type=int, default=None)
    parser.add_argument("--index", type=int, default=None)
    args = parser.parse_args()
    
    config_file = "./Config/"+args.dataset.lower()+"_config.ini"
    
    config = configparser.ConfigParser()
    config.read(config_file)
    
    dataset = config['default']['dataset']
    vae_arch = config['default']['vae_arch']
    #latent_dim is the number of latent dimensions of the VAE including noise and non-noise
    latent_dim = int(config['default']['latent_dim'])
    vae_sample = int(config['default']['vae_sample'])
    partitions = int(config['default']['partitions'])
    ways = int(config['default']['ways'])
    batch_size = int(config['default']['batch_size'])
    
    #info_dim is the number of non-noise dimensions
    info_dim = int(config['VAE_'+str(vae_sample)]['info_dim'])
    #noise is a list containing the indices of the noise dimensions in the original latent size
    noise = config['VAE_'+str(vae_sample)]['noise'] #This is a str
    noise = convert_string_tolist(noise)
    
    if args.ways:
        ways = args.ways
        print(f"Running the script for {ways}-way combinations")
    if args.partitions:
        partitions = args.partitions 
        print(f"Running the script for {partitions} partitions")
        
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu") 
    vae = load_vae(config).to(device)
    vae_suffix = f"_{vae_sample}"
    index_suffix = f"_{args.index}" if args.index else ""
    
    density_suffix=f"_{args.density_range}" if args.density_range else "" 
        
    assert vae_arch == "TwoStage", "This script works only for a TwoStage VAE"

    assert os.path.exists(f"./Results/{dataset}_{vae_arch}{vae_suffix}/sample_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.txt"), f"latent vector file ./Results/{dataset}_{vae_arch}{vae_suffix}/sample_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.txt not generated"
    latent_vectors = np.loadtxt(f"./Results/{dataset}_{vae_arch}{vae_suffix}/sample_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.txt", delimiter=" ")
    no_samples = latent_vectors.shape[0]
        
    ts_size = latent_vectors.shape[0]
    
    for n in noise:
        latent_vectors = np.insert(latent_vectors, n, np.random.normal(0,1,latent_vectors.shape[0]), axis=1)
    
    latent_vectors = torch.from_numpy(latent_vectors).to(torch.float32)
    my_dataset = TensorDataset(latent_vectors,latent_vectors)
    my_dataloader = DataLoader(my_dataset, batch_size = batch_size, shuffle=False)
    
    vae.eval()
    init = True
    with torch.no_grad():
        for data,_ in tqdm(my_dataloader, disable=True):
            data = data.to(device)
            if init:
                ts = vae.decoder(vae.decoder2(data))
                init = False
            else:
                temp = vae.decoder(vae.decoder2(data))
                ts = torch.cat((ts, temp), 0)
    ts = ts.detach().to(torch.device("cpu"))
    #randomly select image panel to display
    indices = [random.randint(0,ts_size-1) for i in range(16)]
    save_image(ts[indices],f'./Results/SampleInputs/{dataset}{density_suffix}.png')
    
    ts = ts.numpy()
    if dataset == "Udacity":
        IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_CHANNELS = 160, 320, 3
        result_ts = np.zeros((ts.shape[0],IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_CHANNELS), dtype=np.float32)
        ts = (np.moveaxis(ts, 1,-1)*255).astype('uint8')
        assert ts.shape[-1] == 3, "incorrect shape"
        for i in range(ts.shape[0]):
            temp = cv2.resize(ts[i], (IMAGE_WIDTH, IMAGE_HEIGHT), cv2.INTER_AREA)
            temp = cv2.cvtColor(temp, cv2.COLOR_RGB2YUV)
            result_ts[i] = temp
        ts = np.moveaxis(result_ts, 3,1)
        print(ts.shape, np.amax(ts), np.amin(ts))
    np.save(f"./Results/{dataset}_{vae_arch}{vae_suffix}/ts_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.npy", ts)
    print(f"TSGENTIME {args.density_range} {time.time()-start} seconds") #test set generation time
    print(f"TSSIZE {args.density_range} {ts.shape[0]}") #no of tests generated