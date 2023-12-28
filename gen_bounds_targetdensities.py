import numpy as np
from scipy.stats import chi, norm
import math
import argparse
import configparser

def create_partitions(radout):
    #Below logic divides probability [0,1] into equal density intervals
    #result will be in parts which is a list of tuples
    #each tuple contains lower and upper bounds of random variable for that division
    
    partmax = radout
    
    #probability of each division
    partition_density = 1.0/args.partitions
    
    #For each of the divisions, convert probability density bounds into random variable bounds
    density_low = 0
    density_high = partition_density
    parts = list()
    for i in range(args.partitions):
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
        
    with open("./Config/partitions_z"+str(dim)+"_p"+str(int(args.partitions))+suffix+".txt", 'w') as partitions_file:
        for part in parts:
            partitions_file.write(str(part[0])+" "+str(part[1])+"\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--dim', metavar='Dim', type=int, default=9,
                        help='number of dimensions; defaults to 9')
    parser.add_argument('--partitions', metavar='Parts', type=int, default=20)
           
    args = parser.parse_args()
    dim  = args.dim
    
    config = configparser.ConfigParser()
    ref_density = 0.99
    density_levels = [0, .49, .94]
    for l, density2 in enumerate(density_levels):
        section = 'Level'+str(l+1)
        suffix = f"_Level{l+1}"
        suffix1=f"_Level{l+1}_i"
        suffix2=f"_Level{l+1}_o"
        
        config.add_section(section)
        config.set(section, 'Cumulative density', str(abs(ref_density-density2)))

        config.set(section, 'Density1', str(density2))
        config.set(section, 'Density2', str(ref_density))
        
        #Using chi-distribution
        radin, radout = chi.interval(ref_density, dim)
        radin2, radout2 = chi.interval(density2, dim)
        print(f"density range: {density2}-{ref_density}")
        
        print("Using Chi-distribution, radii range for shell 1 ", radin, radin2)
        print("Using Chi-distribution, radii range for shell 2 ", radout2, radout)                
        with open("./Config/boundaries_z"+str(dim)+"_p"+str(int(args.partitions))+suffix1+".txt", 'w') as boundaries_file:
                boundaries_file.write(str(radin)+"\n")
                boundaries_file.write(str(radin2)+"\n")
                
        with open("./Config/boundaries_z"+str(dim)+"_p"+str(int(args.partitions))+suffix2+".txt", 'w') as boundaries_file:
                boundaries_file.write(str(radout2)+"\n")
                boundaries_file.write(str(radout)+"\n")    
        create_partitions(radout)
                
    density_levels = [.9999,.999999]
    prev_density = ref_density
    for l, density2 in enumerate(density_levels):
        section = 'Level'+str(l+4)
        suffix = f"_Level{l+4}"
        suffix1=f"_Level{l+4}_i"
        suffix2=f"_Level{l+4}_o"
        
        config.add_section(section)
        config.set(section, 'Cumulative density', str(abs(prev_density-density2)))

        config.set(section, 'Density1', str(prev_density))
        config.set(section, 'Density2', str(density2))
        #Using chi-distribution
        radin, radout = chi.interval(density2, dim)
        radin2, radout2 = chi.interval(prev_density, dim)
        print(f"density range: {prev_density}-{density2}")
        prev_density = density2
        print("Using Chi-distribution, radii range for shell 1 ", radin, radin2)
        print("Using Chi-distribution, radii range for shell 2 ", radout2, radout)                
        with open("./Config/boundaries_z"+str(dim)+"_p"+str(int(args.partitions))+suffix1+".txt", 'w') as boundaries_file:
                boundaries_file.write(str(radin)+"\n")
                boundaries_file.write(str(radin2)+"\n")
                
        with open("./Config/boundaries_z"+str(dim)+"_p"+str(int(args.partitions))+suffix2+".txt", 'w') as boundaries_file:
                boundaries_file.write(str(radout2)+"\n")
                boundaries_file.write(str(radout)+"\n")   
        create_partitions(radout)
                
    with open(f"Config/densities_z{dim}.ini", 'w') as f:
        config.write(f)
        
            