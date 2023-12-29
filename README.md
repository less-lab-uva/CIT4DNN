# CIT4DNN

This repository contains the official implementation of CIT4DNN, a black-box test generation method for DNNs proposed in our paper "CIT4DNN: Generating Diverse and Rare Inputs for Neural Networks Using Latent Space Combinatorial Testing".

## Datasets and Models
The artifact supports MNIST, FashionMNIST, SVHN, TaxiNet, and Udacity datasets. Pretrained Variational autoencoder (VAE) and DNN models for the MNIST dataset are available at [VAE](https://github.com/less-lab-uva/CIT4DNN/tree/main/VAE) and [DNN](https://github.com/less-lab-uva/CIT4DNN/tree/main/DNN) directories respectively. These directories also include scripts for training the models. 
Pretrained DNN and VAE models for all the five datasets are available in zenodo, and they should be copied to the appropriate directories for running the full studies.

## Setup
We used Python 3.8 for running the experiments. Use the below instructions for creating the virtual environment for running the scripts.
- Create virtual environment

    `python -m venv cit4dnn`
- Activate virtual environment

    `source cit4dnn/bin/activate`
- Install the required packages using the `requirements.txt` file.

    `pip install -r requirements.txt`

## Usage
`run_mnist.sh` runs the scripts for the MNIST dataset for quickly replicating the research results. This experiment instantiates CIT4DNN with a choice of configuration paramters, generates test inputs from radial-constrained covering arrays, and measures the faults using a differential test oracle. The experiment is run for five target densities {D1, D2, D3, D4, D5} as described in Section 4.2 of the paper.

The bash script runs the experiment only 2 times in the interest of time. Please note that the script is run 10 times and average values are reported in the experiments presented in the paper.
The script prints out the average number of faults present in the generated test sets as the final output which should approximately match the first row of Table 3 in the paper.
 
The script runs for about 15 minutes and should display the below output. The runtime logs are saved in `Results/Logs` directory and the output in `Reports` directory. The script also saves sample test inputs as image files in `Results/SampleInputs` directory.

```
./run_mnist.sh
	
Running tests for MNIST
Running iteration 1
Running experiments for MNIST
Generating latent samples for Level1
Generating test set for Level1
Meauring faults for Level1
.....
.....
.....
Generating report...
[Dataset, Metric, D1, D2, D3, D4, D5, D5/D1]
['MNIST', '#tests', 14658, 14668, 15421, 15825, 16073, 1.1]
['MNIST', '#faults', 748, 789, 1291, 1782, 3134, 4.19]
```

The script can also be run on a docker. Execute the below commands to build the docker image from the provided Dockerfile and run the container.

```
docker build -t cit4dnn .
docker run cit4dnn
```

The full study can be run by using `./run_tests.sh`. This script runs the above experiment for all the five datasets used in the paper and repeats the experiment 10 times.
Please note that this script runs for many hours. The result of this script is the complete Table 3 presented in the paper.

```
./run_tests.sh
```

## Modules of CIT4DNN
CIT4DNN implementation has three major modules, Constrained Combinatorial Interaction Testing (CCIT), Sample Partition, and Generator for generating Radial Constrained Covering Arrays (RCCA), latent samples, and test inputs respectively.

### CCIT
CCIT is implemented by extending greedy AETG-SAT covering array generation algorithm<sup>[[1]](#1)</sup> with an SMT solver to handle the radial constraints formulated over the geometry of the latent space of a variational autoencoder. This module generates RCCA representing the test descriptions and the implementation is present in the [AETG](https://github.com/less-lab-uva/CIT4DNN/tree/main/AETG) directory of the artifact. The results are saved in the `Results` directory, including RCCA and runtime logs with the metadata and the execution time. 

The AETG-SAT algorithm is proposed by Myra Cohen, Matthew Dwyer, and Jiangfan Shi, and we thank them for sharing the implementation. The primary AETG algorithm<sup>[[2]](#2)</sup> is implemented by Myra Cohen and the SAT part of the algorithm is contributed by Jiangfan Shi. We added the SMT solver part to the algorithm in order to support the radial constraints used by CIT4DNN.

### Sample Partition
`gen_latent_samples.py` implements the Sample Partition module. 
This script generates latent samples corresponding to the test descriptions in RCCA. These latent samples are saved in the `Results` directory along with the log files containing the runtime.

### Generator
`gen_test_inputs.py` implements the logic for generating test inputs from the latent samples using the decoder of the VAE. The test inputs are saved in the `Results` directory along with the log files including runtime information and a sample of image files corresponding to the generated test inputs.

## References
<a id="1">[1]</a> Cohen, Myra B., Matthew B. Dwyer, and Jiangfan Shi. "Constructing interaction test suites for highly-configurable systems in the presence of constraints: A greedy approach." IEEE Transactions on Software Engineering 34.5 (2008): 633-650.

<a id="2">[2]</a> Cohen, Myra B., et al. "Constructing test suites for interaction testing." 25th International Conference on Software Engineering, 2003. Proceedings.. IEEE, 2003.
