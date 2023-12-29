import re
import sys
import csv
from statistics import mean
import numpy as np

datasets = sys.argv[1:-1]
no_repetitions = int(sys.argv[-1])

mean_rows = []
for dataset in datasets:
    mean_row1 = [dataset,"#tests"]
    mean_row2 = [dataset,"#faults"]
    for l in range(1,6): #5 levels
        with open(f"Reports/{dataset}_D{l}.csv", 'w') as csvfile:
            csvwriter = csv.writer(csvfile)
            csvwriter.writerow(["Run","TestsetSize", "Faults"])
            
            row = []
            lv_time = []
            fault_time = []
            lv_size = []
            faults_size = []
            for i in range(1,no_repetitions+1): #n repetitions of the experiment 
                row.append(i)
                with open(f"Results/Logs/{dataset}_results_{i}.txt", 'r') as log_file:
                    log_content = log_file.read()
                    p = re.compile(f'LVSIZE Level{l} (\d+)')
                    latent_size = int(p.findall(log_content)[0])
                    
                    p = re.compile(f'TSSIZE Level{l} (\d+)')
                    test_size = int(p.findall(log_content)[0])
                    assert test_size == latent_size, "incorrect latent and test sizes"
                    lv_size.append(test_size)
                    
                    p = re.compile(f'FAULTSSIZE Level{l} (\d+)')
                    faults_size.append(int(p.findall(log_content)[0]))
            mean_row1 += [int(mean(lv_size))]
            mean_row2 += [int(mean(faults_size))]
            row = np.concatenate((np.array(row).reshape(1,-1), np.array(lv_size).reshape(1,-1), np.array(faults_size).reshape(1,-1)), axis=0).T
            row = row.tolist()
            csvwriter.writerows(row)
    mean_row1 += [round(mean_row1[6]/mean_row1[2],2)]
    mean_row2 += [round(mean_row2[6]/mean_row2[2],2)]
    mean_rows.append(mean_row1)
    mean_rows.append(mean_row2)
            
with open(f"Reports/rq3.csv", 'w') as csvfile:
    csvwriter = csv.writer(csvfile)
    csvwriter.writerow(["Dataset", "Metric","D1", "D2", "D3", "D4", "D5", "D5/D1"])
    csvwriter.writerows(mean_rows)
    
print("[Dataset, Metric, D1, D2, D3, D4, D5, D5/D1]")
for i in range(len(mean_rows)):
    print(mean_rows[i])
            
            
                
                 
                
