#Use this script to try the command multiple times

date

for i in {1..100}
do
    echo Trial: $i/100 `date`
    make srun
done
echo finish


date
