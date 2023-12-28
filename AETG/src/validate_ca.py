import numpy as np
from scipy.stats import chi, norm
import math
import argparse

def calculate_norm(covering_array):
    norm = np.linalg.norm(covering_array, ord=2, axis=1)
    
    #or another way to calculate L2 norm
    #norm = np.sqrt(np.square(covering_array).sum(axis=1))
    
    return norm
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--dim', metavar='Dim', type=int, default=10,
                        help='number of dimensions; defaults to 10')
    parser.add_argument('--numparts', metavar='Parts', type=int, default=10,
                        help='number of partitions of positive coordinates; defaults to 4 meaning that there are 8 total partitions')
    parser.add_argument('--ways', type=int, default=3)
    parser.add_argument('--strategy', type=str, default="uniform", choices=["uniform", "random", "mean", "max_density"])
    parser.add_argument('--density', metavar='Density', type=float, default=0.9999,
                        help='density inside the annulus')
           
    args = parser.parse_args()

    dim = args.dim
    numparts = args.numparts
    density = args.density
    sample_file = f"sample_{args.strategy}_z{dim}_p{2*numparts}_t{args.ways}.npy"
    assert density < 1.0, "density cannot be greater than 1, also avoiding equals 1 case as the bound will be inf"
        
    #Using chi-distribution
    radin, radout = chi.interval(density, dim)
    print("Using Chi-distribution, annulus range ", radin, radout)
    #Below logic divides probability [0,1] into equal density intervals
    #result will be in parts which is a list of tuples
    #each tuple contains lower and upper bounds of random variable for that division
    
    partmax = radout
    
    #probability of each division
    partition_density = 1.0/(2*numparts)
    
    #For each of the divisions, convert probability density bounds into random variable bounds
    density_low = 0
    density_high = partition_density
    parts = list()
    for i in range(2*numparts):
        #density can be greater than 1 by a negligible amount due to 
        #density_high variable rhs in the below logic
        if density_high > 1:
            density_high = 1
            
        #below code converts probability bounds to random variable 
        #bounds using quantile function
        rv_low = norm.ppf(density_low)
        rv_high = norm.ppf(density_high)
        
        #below logic limits the random variable partitions to 
        #lie in [-partsmax, partsmax] range
        if rv_low == -np.inf:
            rv_low = -partmax
        elif rv_high > partmax:
            rv_high = partmax
            
        if rv_low >= rv_high:
            print("Error in dividing probability densities")
            exit(0)
        
        if math.isclose(rv_low, 0, abs_tol=1e-5):
            rv_low = 0
        if math.isclose(rv_high, 0, abs_tol=1e-5):
            rv_high = 0
        parts.append((rv_low, rv_high))
            
        density_low = density_high
        density_high = density_high + partition_density
    
    ca = np.load(sample_file)
    assert ca.shape[1] == dim, "incorrect latent dimension size"
    print(f"covering array has {ca.shape[0]} samples")
    dist = calculate_norm(ca)
    
    outliers = (dist < radin) | (dist > radout)
    num_outliers = np.sum(outliers)
    print(f"Number of samples that donot satisfy the shell constraints: {num_outliers}")   
    if num_outliers > 0:
        print(f"outlier distance from the origin {calculate_norm(ca[outliers])}")
    