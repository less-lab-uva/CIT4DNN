import numpy as np
from sklearn.metrics import accuracy_score, mean_squared_error
from Model2_mnist import Model2_mnist
from Model3_mnist import Model3_mnist
import os
if os.path.exists("./Model1_fmnist.py"): #below files are available in zenodo
    from Model1_fmnist import Model1_fmnist
    from Model2_fmnist import Model2_fmnist
    from Model1_svhn import Model1_svhn
    from Model2_svhn import Model2_svhn
    from Model1_taxinet import * 
    import Model1_udacity
    import Model3_udacity
from keras.layers import Input
from keras.datasets import mnist
import argparse
import configparser
import cv2
import keras
import time

start = time.time()
parser = argparse.ArgumentParser()
parser.add_argument("--dataset", type=str, default="MNIST", choices=["MNIST", "FashionMNIST", "SVHN", "Udacity", "TaxiNet"])
parser.add_argument("--density_range", type=str, default=None)
parser.add_argument("--strategy", type=str, default="density", choices=["density", "random"])
parser.add_argument("--factor", type=int, default=1)
parser.add_argument("--ways", type=int, default=None)
parser.add_argument("--index", type=int, default=None)
args = parser.parse_args()

print("working with dataset...", args.dataset)
    
config_file_path = "../Config/"+args.dataset.lower()+"_config.ini"
config = configparser.ConfigParser()
config.read(config_file_path)

dataset = config['default']['dataset']
vae_arch = config['default']['vae_arch']
#latent_dim is the number of latent dimensions of the VAE including noise and non-noise
vae_sample = int(config['default']['vae_sample'])
partitions = int(config['default']['partitions'])
info_dim = int(config["VAE_"+str(vae_sample)]['info_dim'])
ways = int(config['default']['ways'])
if args.ways:
    ways = args.ways
    
density_suffix = f"_{args.density_range}" if args.density_range else ""
vae_suffix = f"_{vae_sample}"
index_suffix = f"_{args.index}" if args.index else ""
    
if dataset == "MNIST":
    input_tensor = Input(shape=(28,28,1))
    model1 = Model2_mnist(input_tensor=input_tensor)
    model2 = Model3_mnist(input_tensor=input_tensor)
elif dataset == "FashionMNIST":
    input_tensor = Input(shape=(28,28,1))
    model1 = Model1_fmnist(input_tensor=input_tensor)
    model2 = Model2_fmnist(input_tensor=input_tensor)
elif dataset == "SVHN":
    input_tensor = Input(shape=(32,32,3))
    model1 = Model1_svhn(input_tensor=input_tensor)
    model2 = Model2_svhn(input_tensor=input_tensor)
elif dataset == "Udacity":
    model1 = Model1_udacity.build_model()
    model1.load_weights('./Model1_udacity.h5')
    model2 = Model3_udacity.build_model()
    model2.load_weights('./Model3_udacity.h5')
else:
    model1 = TaxiNet32x16()
    model1.load_state_dict(torch.load("Model1_taxinet.pth"))
    model = TaxiNet32x16()
    model2 = TaxiNet32x16_Model2(model)
    model2.load_state_dict(torch.load("Model2_taxinet.pth"))

x_test = np.load(f"../Results/{dataset}_{vae_arch}{vae_suffix}/ts_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.npy")
if dataset in ["MNIST", "FashionMNIST", "SVHN", "TaxiNet"]:
    assert np.amax((x_test*255).astype('uint8')) >= 250 and np.amax((x_test*255).astype('uint8')) <= 255, f"incorrect normalization {np.amax((x_test*255).astype('uint8'))}"
else:
    assert np.amax(x_test) >= 250 and np.amax(x_test) <= 255, f"incorrect normalization {np.amax(x_test)}"
    
x_test = x_test.astype('float32')
if dataset in ["MNIST", "FashionMNIST"]:
    x_test = x_test.reshape(-1, 28, 28, 1)
elif dataset in ["SVHN", "Udacity"]:
    x_test = np.moveaxis(x_test, 1, 3)
    
print(f"Shape of the testset is {x_test.shape} min {np.amin(x_test)} max{np.amax(x_test)}")

#find the model predictions for the testset
if dataset in ["TaxiNet"]:
    pred1 = model1(torch.from_numpy(x_test).float()).detach().numpy()
    pred2 = model2(torch.from_numpy(x_test).float()).detach().numpy()
else:
    pred1 = model1.predict(x_test)
    pred2 = model2.predict(x_test)

if dataset in ["MNIST", "FashionMNIST", "SVHN"]:
    pred1 = np.argmax(pred1, axis=1)
    pred2 = np.argmax(pred2, axis=1)
    
np.savetxt(f"../Results/{dataset}_{vae_arch}{vae_suffix}/pred_model1_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.txt", pred1, delimiter=" ")
np.savetxt(f"../Results/{dataset}_{vae_arch}{vae_suffix}/pred_model2_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.txt", pred2, delimiter=" ")

#Measure the number of faults in the samples
if dataset in ["MNIST", "FashionMNIST", "SVHN"]:
    no_matches = (pred1 == pred2).sum()   
else:
    if dataset == "Udacity":
        delta = 0.1 #(5%)
    else:
        delta = 3.5 #5% of test range
        pred1 = pred1[:,1]
        pred2 = pred2[:,1]
    faults = 0 
    for i in range(pred1.shape[0]):
        angle1 = pred1[i]
        angle2 = pred2[i]
        if abs(angle1 - angle2) > delta and not ((angle1 > 0 and angle2 > 0) or (angle1 < 0 and angle2 < 0)):
            faults += 1
    no_matches = pred2.shape[0] - faults
print(f"Number of matches {no_matches} out of {pred2.shape[0]}")
print(f"Number of faulty tests {pred2.shape[0]-no_matches} out of {pred2.shape[0]}")
print(f"FAULTSGENTIME {args.density_range} {time.time()-start} seconds")
print(f"FAULTSSIZE {args.density_range} {pred2.shape[0]-no_matches}")
if dataset in ["MNIST", "SVHN", "FashionMNIST"]:
    indices = np.not_equal(pred1, pred2)   
    fault_indices = np.array(range(pred1.shape[0]))[indices]
    np.save(f"../Results/{dataset}_{vae_arch}{vae_suffix}/faults_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.txt", fault_indices)

