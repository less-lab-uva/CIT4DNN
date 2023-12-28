import sys
import os
import numpy as np
from scipy.stats import norm
import argparse
import configparser
import random
from utils import *
from z3 import *
import time

s = Solver()
Z = list()
#delta to adjust the precision of the approach
delta = 0.00001

def distanceconstraint(partmax):
    global Z, s
    Z = [Real('z_%s' % (i + 1)) for i in range(info_dim)]
    zlowerbound = [Z[i] >= 0 for i in range(info_dim)]
    s.add(zlowerbound)
    zupperbound = [Z[i] < (partmax*partmax) for i in range(info_dim)]
    s.add(zupperbound)
    return

def dropstateconstraint():
    global s
    for i in range(info_dim+1):
        s.pop()
    return

def checkstateconstraint(radius, partition, is_density=None):
    global Z, s
    radin=radius[0]
    radout = radius[-1]
    zsum = 0
    for i in range(info_dim):
        zsum = zsum + Z[i]
    s.push()
    if not is_density:
        s.add(zsum >= (radin*radin)+delta, zsum < (radout*radout))
    else:
        radin2 = radius[1]
        radout2 = radius[2]
        radin_sq = (radin*radin)+delta
        radin2_sq = (radin2*radin2)-delta
        radout2_sq = (radout2*radout2)+delta
        radout_sq = (radout*radout)-delta
        s.add(Or(And(zsum >= radin_sq, zsum < radin2_sq),And(zsum >= radout2_sq, zsum < radout_sq)))
    for depth in range(info_dim):
        s.push()
        if partition[depth,0] < 0:
            s.add(Z[depth] <= (partition[depth,0]*partition[depth,0]), Z[depth] > (partition[depth,1]*partition[depth,1]))
        else:
            s.add(Z[depth] >= (partition[depth,0]*partition[depth,0]), Z[depth] < (partition[depth,1]*partition[depth,1]))
    
    if s.check() == sat:
        dropstateconstraint()
        return s.model()
    else:
        print(f"   with constraint {s.assertions()}")
        print("Solver couldnt find a model")
        exit(0)
    
def genLatentVectors(ca, parts_array, strategy):
    p = parts_array.shape[0]
    parts = dict({})
    density_bounds = dict({})
    for i in range(parts_array.shape[0]):
        parts[i] = tuple(parts_array[i])
        density_bounds[i] = (norm.cdf(parts[i][0]), norm.cdf(parts[i][1]))

    sample = np.empty((ca.shape[0], ca.shape[1]), dtype=float)
    if strategy == "density":
        for row in range(ca.shape[0]):
            for col in range(ca.shape[1]):
                sample[row, col]   = norm.ppf(np.random.uniform(density_bounds[ca[row,col]][0], density_bounds[ca[row,col]][1]))             
    else:
        print("Random sampling")
        #random sampling from latent space
        sample = np.random.normal(0,1,(ca.shape[0], ca.shape[1])).astype(np.float32)
    return sample

def main(args):
    density_suffix=f"{args.density_range}" if args.density_range else ""
    ca = np.loadtxt(f"./Results/CoveringArrays/aetg_z{info_dim}_p{partitions}_t{ways}{density_suffix}.out", dtype=int, delimiter=" ")
    assert ca.shape[1] == info_dim, f"incorrect number of columns in the covering array {ca.shape[1]}"
    
    density_suffix=f"_{args.density_range}" if args.density_range else ""
    
    #parts contains the mappings from partition number to partition 
    #boundaries in the latent space
    parts_array = np.loadtxt(f"./Config/partitions_z{info_dim}_p{partitions}{density_suffix}.txt", dtype=float, delimiter=" ")
    assert parts_array.shape[0] == partitions, f"incorrect partitions_z{info_dim}_p{partitions}{density_suffix}.txt file"
    intervals = np.array([parts_array[0,0]]+list(parts_array[:,1]))
    
    print(f'-------------------Configuration-------------------')
    print(f'No of latent dimensions: {latent_dim}')
    print(f'No of non-noise latent dimensions: {info_dim}')
    print(f'No of partitions on each latent dimension: {partitions}')
    print(f'Strenght of CIT t: {ways}')
    print(f'Sampling strategy: {args.strategy}')
    print(f'Sampling factor: {args.factor}')
    print(f"Number of inputs in the covering array: {ca.shape}")
    print(f'Partitions: {intervals}')
    print(f'---------------------------------------------------')
    
    ca_temp = ca.copy()
    for f in range(args.factor-1):
        shuffle_cols = list(range(info_dim))
        random.shuffle(shuffle_cols)
        ca = np.concatenate((ca, ca_temp[:, shuffle_cols]), axis=0)
    
    latent_vectors = genLatentVectors(ca, parts_array, args.strategy)
    
    if args.density_range:
        bounds = np.loadtxt(f"./Config/boundaries_z{info_dim}_p{partitions}{density_suffix}_i.txt", dtype=float)
        radin = bounds[0]
        radin2 = bounds[1]
        bounds = np.loadtxt(f"./Config/boundaries_z{info_dim}_p{partitions}{density_suffix}_o.txt", dtype=float)
        radout2 = bounds[0]
        radout = bounds[1]
        radius_tuple = (radin, radin2, radout2, radout)
    else:
        bounds = np.loadtxt(f"./Config/boundaries_z{info_dim}_p{partitions}.txt", dtype=float)
        radin = bounds[0]
        radout = bounds[1]
        radius_tuple = (radin, radout)
    distanceconstraint(radout)
    
    _, _, outlier_indices = generate_array(latent_vectors, args.target_density, partitions, args.density_range)
    if not args.density_range:
        outlier_indices = list(outlier_indices[0])+list(outlier_indices[1])
    else:
        outlier_indices = list(outlier_indices[0])+list(outlier_indices[1])+list(outlier_indices[2])
    
    no_outliers = len(outlier_indices)
    print(f"{args.density_range} Outliers to project into the shell {no_outliers}")
    if args.strategy == "density" and len(outlier_indices) > 0:            
        #fix the samples that are outside the target density
        for i in list(outlier_indices):
            sign_vector = np.sign(latent_vectors[i])
            outlier_ca = ca[i]
            parts_ca = np.zeros((info_dim,2), dtype=np.float32)
            for j in range(info_dim):
                parts_ca[j,0] = parts_array[outlier_ca[j],0] +delta
                parts_ca[j,1] = parts_array[outlier_ca[j],1] -delta
            
            m = checkstateconstraint(radius_tuple, parts_ca, args.density_range)
            vector = np.zeros((info_dim), dtype=np.float32)
            for k in range(info_dim):
                e = m[Z[k]]
                vector[k] = math.sqrt(float(e.numerator_as_long())/float(e.denominator_as_long()))
            latent_vectors[i] = vector * sign_vector
            
            
            lv_cov = np.digitize(latent_vectors[i], intervals)
            if not (lv_cov==(ca[i]+1)).all():
                print("SMT failed")
                #Solver sometime fails to return a models that maps to the target bins due to floating point rounding issue
                #adjust delta parameter and run again
                exit(0)
            
        feasible_latent_vectors, valid, outlier_indices = generate_array(latent_vectors, args.target_density, partitions, args.density_range)

        if args.density_range:
            outlier_indices = list(outlier_indices[0])+list(outlier_indices[1])+list(outlier_indices[2])
        else:
            outlier_indices = list(outlier_indices[0])+list(outlier_indices[1])
        print(f"{args.density_range} Outliers after projection {len(outlier_indices)}")
        
    if not os.path.exists(f"./Results/{dataset}_{vae_arch}{vae_suffix}"):
        os.mkdir(f"./Results/{dataset}_{vae_arch}{vae_suffix}")
    np.savetxt(f"./Results/{dataset}_{vae_arch}{vae_suffix}/sample_{args.strategy}{args.factor}_z{info_dim}_p{partitions}_t{ways}{density_suffix}{index_suffix}.txt", latent_vectors, delimiter=" ")
    print(f"LVSIZE {args.density_range} {latent_vectors.shape[0]}")  #Size of generated latent vectors
    print(f"Outliers fixed by SMT {no_outliers}")
    
if __name__ == "__main__":    
    start = time.time()
    parser = argparse.ArgumentParser()
    parser.add_argument("--dataset", type=str, choices=["MNIST", "SVHN","TaxiNet", "Udacity", "UnityEyes", "FashionMNIST"])
    parser.add_argument("--factor", type=int, default=1)
    parser.add_argument("--strategy", type=str, default="density", choices=["density"])
    parser.add_argument("--density_range", type=str, default=None)
    parser.add_argument("--target_density", type=float, default=.9999) #when d_i is zero
    parser.add_argument("--ways", type=int, default=None)
    parser.add_argument("--partitions", type=int, default=None)
    parser.add_argument("--index", type=int, default=None) #for repeating the experiemnts multiple times
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
    
    #info_dim is the number of non-noise dimensions
    info_dim = int(config['VAE_'+str(vae_sample)]['info_dim'])
    
    if args.ways:
        ways = args.ways
        print(f"Running the script for {ways}-way combinations")
    if args.partitions:
        partitions = args.partitions 
        print(f"Running the script for {partitions} partitions")
        
    vae_suffix = f"_{vae_sample}"
    index_suffix = f"_{args.index}" if args.index else ""
    main(args)
    print(f"LVGENTIME {args.density_range} {time.time()-start} seconds")
