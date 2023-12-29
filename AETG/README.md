# Constrained Combinatorial Interaction Testing

# Setup
Running this program requires GCC and z3 solver installed.

Follow the instruction at https://github.com/Z3Prover/z3/tree/master for installing the z3 solver using the C++ compiler.

Run the below commands to update the path variables.
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path to z3 on your machine>/z3/build/

export PATH=$PATH:<path to z3>/z3/build/:<path to z3 on your machine>/z3/build/api

export PYTHONPATH=$PYTHONPATH:<path to z3>/z3/build/python
```
Build `maetg` executable using the below commands.
```
cd src
make clean
make
```

# Usage

## CIT Parameters used by CIT4DNN:
- t: strength of CIT 
- factors: the number of non-noise latent dimensions of the VAE
- levels: number of partitions along each latent dimension

## Running CCIT
```
./maetg <inputs file> <avoids file> -r <repetitions> -d <target density tag>

inputs file: This file requires CIT configuration.
The first line of the file should be the strength t of CIT.
The second line should be the number of factors of CIT.
The third line has two space separated entities, 1) number of levels and 2) number of factors of CIT.

Example inputs file:
Please refer to src/idc-inputs-9.txt for an instanciation of the inputs file for the MNIST experiment.
RCCA is generated for strength t=3, the number of non-noise latent dimensions of MNIST VAE is 9, and each latent dimension is partitioned into 20 intervals. 

avoids file: This is required for running the AETG-SAT algorithm which is not used by CIT4DNN.
Use the provided empty file, idc-avoids.txt, for this argument.

repetitions: The original algorithm runs r times and selects the best covering array. This can be left as 1 for the CIT4DNN experiments.

target density tag: This parameter refers to the target density mentioned in the paper.
We used five target densities, D1-D5, in the paper.
These target densitie tags Level[1-5] are used for D1-D5 respectively in the experiments.

The target density files have to be generated using [gen_bounds_targetdensities.py](https://github.com/less-lab-uva/CIT4DNN/tree/main/gen_bounds_targetdensities.py) script. Refer to [README.md](https://github.com/less-lab-uva/CIT4DNN/tree/main/README.md) file for instructions about running this script.
```

This program generates a log file, aetglog_z\<factors\>\_p\<levels\>\_t\<t\>\<target density tag\>.out, and an RCCA, aetg_z\<factors\>\_p\<levels\>\_t\<t\>\<target density tag\>.out in the [Results/CoveringArrays](https://github.com/less-lab-uva/CIT4DNN/tree/main/Results/CoveringArrays) directory.

## Example 
Below command is used for generating RCCA for the MNIST experiments for the target density D1 mentioned in the paper.

```
./maetg idc-inputs-9.txt idc-avoids.txt -r 1 -d Level1

setting options 
Changed REPEAT to 1
Changed density_range to Level1
ending setting of options 
radin 1.15949 radout 2.71
radin2 2.71 radout2 4.68561
aetg cover t   k   v  TCount  
3 8 160 448000

 20 20 20 20 20 20 20 20

  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19
  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39
  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59
  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79
  80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95  96  97  98  99
  100  101  102  103  104  105  106  107  108  109  110  111  112  113  114  115  116  117  118  119
  120  121  122  123  124  125  126  127  128  129  130  131  132  133  134  135  136  137  138  139
  140  141  142  143  144  145  146  147  148  149  150  151  152  153  154  155  156  157  158  159
radin 1.15949 radout 2.71
radin2 2.71 radout2 4.68561
Repeat 0
```

The above program generates aetlog_z9_p20_t3Level1.out and aetg_z9_p20_t3Level1.out in the [Results/CoveringArrays](https://github.com/less-lab-uva/CIT4DNN/tree/main/Results/CoveringArrays) directory.
