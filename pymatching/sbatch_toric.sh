#!/bin/bash
#SBATCH --time=1-00:00:00
#SBATCH -n 1
#SBATCH --cpus-per-task=31

source pymatching-env/bin/activate

python toric.py --num_trials=10000000 --num_errors_max=100 --pool_size=31

