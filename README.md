# CIT4DNN

This repository contains the official implementation of CIT4DNN, a black-box test generation method for DNNs proposed in our paper "CIT4DNN: Generating Diverse and Rare Inputs for Neural Networks Using Latent Space Combinatorial Testing".

## Datasets and Models
The artifact supports MNIST, FashionMNIST, SVHN, TaxiNet, and Udacity datasets. Pretrained Variational autoencoder (VAE) and DNN models for all the five datasets are available in [VAE](https://github.com/less-lab-uva/CIT4DNN/tree/main/VAE) and [DNN](https://github.com/less-lab-uva/CIT4DNN/tree/main/DNN) directories respectively. These directories also include scripts for training the models. Add zenodo link,,,

## Setup
We used Python 3.8 for running the experiments. Use the below instructions for creating the virtual environment for running the scripts.
- Create virtual environment

    `python -m venv cit4dnn`
- Activate virtual environment

    `source cit4dnn/bin/activate`
- Install the required packages using the `requirements.txt` file.

    `pip install -r requirements.txt`

The scripts can also be run on a docker. Execute the below command to build the docker image from the provided Dockerfile.

`docker build -t cit4dnn .`

## Usage
`run_mnist.sh` script runs the experiments for quickly replicating the research results. This experiment instantiates CIT4DNN with a choice of configuration paramters, generates test inputs from radial-constrained covering arrays, and measures the faults using a differential test oracle. The experiment is run for five target densities {D1, D2, D3, D4, D5} as described in Section 4.2 of the paper.

The bash script runs the experiment only 2 times in the interest of time. Please note that the script is run 10 times and average values are reported in the experiments presented in the paper.
The script prints out the average number of faults present in the generated test sets as the final output which should approximately match the first row of Table 3 in the paper.
 
The script runs for about 15 minutes and should display the below output. The runtime logs are saved in Results/Logs directory and the output in Reports directory. The script also saves sample test inputs in Results/SampleInputs directory.

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
## Components

1. Radial Covering Array Generator
2. Latent Sample Generator
3. Test Generator

### Radial Covering Array Generator

### Latent Sample Generator

### Test Generator
