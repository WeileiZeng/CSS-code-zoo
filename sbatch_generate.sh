#!/bin/bash
# check locally: bash sbatch_generate.sh

#SBATCH --job-name=generate_array
#SBATCH --mail-type=ALL
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=06:00:00
#SBATCH --array=21-25%4
#SBATCH --partition=small
#SBATCH --output=log/generate_array_%A_%a.log


echo "SLURM_JOBID: " $SLURM_JOBID
echo "SLURM_ARRAY_TASK_ID: " $SLURM_ARRAY_TASK_ID
echo "SLURM_ARRAY_JOB_ID: " $SLURM_ARRAY_JOB_ID

#n_start=$SLURM_ARRAY_TASK_ID
#(( n_end = $SLURM_ARRAY_TASK_ID  + 1 ))
(( num_cores = 16 ))

srun ./generate_css_code.out num_cores=$num_cores \
    code_folder="../data/CSS-Codes/run2/" \
    n_start="$((SLURM_ARRAY_TASK_ID))" \
    n_end="$((SLURM_ARRAY_TASK_ID+1))" \
    num_trials="$((num_cores))"
