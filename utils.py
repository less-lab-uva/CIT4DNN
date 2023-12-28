import numpy as np
import csv
import subprocess
import re
import os
from scipy.stats import chi, norm
            
def convert_string_tolist(s):
    s = s.replace("[", "")
    s = s.replace("]", "")
    s = s.replace(" ", "")
    return list(map(int, s.split(",")))
   
def create_intervals(dims, no_bins, latent_range):    
    #Below logic divides probability [0,1] into equal density intervals
    #result will be in parts which is a list of tuples
    #each tuple contains lower and upper bounds of random variable for that division
    
    #probability of each division
    partition_density = 1.0/(no_bins)
    
    #For each of the divisions, convert probability density bounds into random variable bounds
    density = 0
    parts = list()
    for i in range(no_bins+1):
        #below if condition is to take care of the cases where density = 1 + epsilon
        #i.e., density is greater than 1 by negligible amount due to 
        #density variable rhs in the below logic
        if density > 1:
            density = 1
            
        #below code converts probability bounds to random variable 
        #bounds using quantile function
        rv = norm.ppf(density)
        
        #below logic limits the random variable partitions to 
        #lie in [-partsmax, partsmax] range
        if rv == -np.inf:
            rv = latent_range[0]
        elif rv > latent_range[1]:
            rv = latent_range[1]
            
        parts.append(rv)

        density = density + partition_density
    
    intervals = np.array(parts)

    return intervals

def measure_coverage(feature_array, acts, ways=3, timeout=10, suffix="temp"):
    # create csv files for calculating IDC
    csv_header = ""
    for i in range(feature_array.shape[1]):
        csv_header += "p" + str(i + 1)
        if i < feature_array.shape[1] - 1:
            csv_header += ","
    csv_file = os.path.join(os.path.dirname(__file__), f"temp/CA_{suffix}.csv")
    np.savetxt(csv_file, feature_array, delimiter=",", header=csv_header, fmt="%d")
    ccmcl_file =  os.path.join(os.path.dirname(__file__),"ccmcl.jar")
    command = [
        "java",
        "-jar",
        str(ccmcl_file),
        "-A",
        str(acts),
        "-I",
        str(csv_file),
        "-T",
        str(ways),
    ]
    print(command)
    output = ""
    process = subprocess.Popen(command, stdout=subprocess.PIPE)
    running = True
    while running:
        for line in process.stdout:
            line = line.decode("utf-8")
            if "Total" in line:
                coverage = float(line.split(" ")[-1])
                process.terminate()
                process.wait()
                running = False
    return coverage
    
#k = factors, v = levels
#dataset str name
#returns name of the generated acts file
def create_acts(k, v):
    #Create acts parameter file
    acts = os.path.join(os.path.dirname(__file__),f"Config/{k}params_{v}bins.txt")
    acts_script = os.path.join(os.path.dirname(__file__),"create_acts.sh")
    #create parameter file for ccmcl tool
    subprocess.call([str(acts_script), "IDC", str(k), '1', str(v), str(acts)])
    assert os.path.exists(acts), "acts not generated"
    return acts
      
def generate_array(latent, density, no_bins=20, density_range=None):
    if density_range: 
        #This is used for rare event testing
        #bounds_i contains the radii of the inner shell and 
        #bounds_o contains the radii of the outer shell
        bounds_i = np.loadtxt(os.path.join(os.path.dirname(__file__),f"Config/boundaries_z{latent.shape[1]}_p{no_bins}_{density_range}_i.txt"), dtype=float, delimiter=" ")
        bounds_o = np.loadtxt(os.path.join(os.path.dirname(__file__),f"Config/boundaries_z{latent.shape[1]}_p{no_bins}_{density_range}_o.txt"), dtype=float, delimiter=" ")
        radin = bounds_i[0]
        radin2 = bounds_i[1]
        radout2 = bounds_o[0]
        radout = bounds_o[1]
    else:
        #calculate annulus boundary based on density
        radin, radout = chi.interval(density, latent.shape[1])
    
    latent_range = (-radout, radout)
    
    #*********************
    intervals = create_intervals(latent.shape[1], no_bins, latent_range)

    x_squares = np.square(latent)
    radius_vector = np.sqrt(np.sum(x_squares, axis=1)).reshape(-1,1)
    indices1 = np.argwhere(radius_vector < radin)[:, 0]
    indices2 = np.argwhere(radius_vector > radout)[:, 0]
 
    if not density_range:
        temp = list(indices1)+list(indices2)
    else:
        indices3 = np.argwhere((radius_vector > radin2) & (radius_vector < radout2))[:, 0]
        temp = list(indices1)+list(indices2)+list(indices3)
    latent = np.delete(latent, temp, axis=0) #delete outliers
    
    #Finding array of partition mappings in the latent space for the latent vectors
    cov_array = np.digitize(latent[:, 0], intervals).reshape(-1, 1)
    for i in range(latent.shape[1]-1):
        cov_vector = np.digitize(latent[:, i+1], intervals).reshape(-1, 1)
        cov_array = np.concatenate((cov_array, cov_vector), axis=1)
    if not density_range:
        return cov_array, latent.shape[0], (indices1,indices2)
    else:
        return cov_array, latent.shape[0], (indices1,indices2, indices3)