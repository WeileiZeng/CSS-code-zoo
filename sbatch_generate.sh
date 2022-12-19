#!/bin/bash
# check locally: bash sbatch_generate.sh

#SBATCH --job-name=generate_array
#SBATCH --mail-type=ALL
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=16
#SBATCH --time=6:00:00
#SBATCH --array=4-30%1
#SBATCH --partition=small
#SBATCH --qos=expedite #expedite 6 hours, short 2 hours
#SBATCH --output=log/%x_%A_%a.log
 #SBATCH --error=log/%x_%A_%a.err
 #SBATCH --nice=10 #larger number with smaller priority
 #SBATCH --begin=23:00:00
 #SBATCH --test-only

# echo "n=30, k=1,2,3"

echo "SLURM_JOBID: " $SLURM_JOBID
echo "SLURM_ARRAY_TASK_ID: " $SLURM_ARRAY_TASK_ID
echo "SLURM_ARRAY_JOB_ID: " $SLURM_ARRAY_JOB_ID
echo "SLURM_JOB_ACCOUNT: " $SLURM_JOB_ACCOUNT
echo "SLURM_JOB_NAME: " $SLURM_JOB_NAME
echo "SLURM_NTASKS: " $SLURM_NTASKS
echo "SLURMD_NODENAME: " $SLURMD_NODENAME
echo "SLURM_PROCID: " $SLURM_PROCID
date

#n_start=$SLURM_ARRAY_TASK_ID
#(( n_end = $SLURM_ARRAY_TASK_ID  + 1 ))
(( num_cores = 16 ))
(( n_start = $SLURM_ARRAY_TASK_ID ))
#(( n_start = 30 ))
srun ./generate_css_code.out num_cores=$num_cores \
    code_folder="../data/CSS-Codes/run2/" \
    n_start="$((n_start))" \
    n_end="$((n_start+1))" \
    num_trials="$((num_cores*100))" \
    note="node-${SLURMD_NODENAME}-proc-${SLURM_PROCID}"

date

# n=5-30, num_cores*10 takes one hour to finish on 4 nodes
