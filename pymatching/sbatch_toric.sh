#!/bin/bash
#SBATCH --job-name="toric"
#SBATCH --time=1-02:00:00
#SBATCH -n 1
 #SBATCH -p small
#SBATCH --cpus-per-task=32

source pymatching-env/bin/activate

date
python -u toric.py --num_trials=100000000 --num_errors_max=100 --pool_size=32
date

#estimated runtime: 3.5 mins for single data, p,L, 1e8. 6 hours to finish all
