make reshape
#define code<k>, code<k>[<d>]
code1=(NA NA NA n7k1d3-x3z3dx3dz3-0 n15k1d4-x8z6dx4dz4-0 n25k1d5-x13z11dx5dz5-0)
code2=(NA NA NA n11k2d3-x4z5dx3dz3-0 n18k2d4-x8z8dx4dz4-0 n27k2d5-x13z12dx5dz5-0)
code3=(NA NA NA n11k3d3-x4z4dx3dz3-0 n19k3d4-x9z7dx4dz4-0 n29k3d5-x13z13dx5dz5-0)

echo "The seed codes used to construct SPC: code<k>[<d>]"
for i in {3..5}; do echo code1$i, ${code1[$i]}; done;
for i in {3..5}; do echo code2$i, ${code2[$i]}; done;
for i in {3..5}; do echo code3$i, ${code3[$i]}; done;

mode=2  #0 for check; 1 for steane; 2 for simulating from file
date 

#i for codeA
for i in {3..5}
do
    #j for codeB
    for j in {3..5}
    do
	if [ $i -le $j ]
	then
	    codeA=${code2[$i]}
	    codeB=${code2[$j]}
	    title=2${i}2${j}
	    # 3 3 get wrong title 2 2
	    echo codeA: ${codeA}, codeB: ${codeB}
	    output=${codeA}_${codeB}
#	    echo ${output}
#	    continue
#	    srun --time=6:00:00 -p small -n 1 --cpus-per-task=16 \
	    srun --time=24:00:00 -n 1 --cpus-per-task=32 \
    ./reshape.out \
    note=${output} \
    title=${title} \
    codeA_prefix=../data/CSS-Codes/run2/${codeA} \
    codeB_prefix=../data/CSS-Codes/run2/${codeB} \
    output=result/unchecked/${output}.json \
    num_cores=32 \
    e_try=100000 \
    debug=0 \
    mode=${mode} &
#	    wait
#	    exit
	fi
    done
done

wait
echo done
exit

srun --time=6:00:00 -p small -n 1 --cpus-per-task=16 \
    ./reshape.out \
    note=${output} \
    codeA_prefix=../data/CSS-Codes/run2/${codeA} \
    codeB_prefix=../data/CSS-Codes/run2/${codeB} \
    output=result/${output}.json \
    num_cores=16 \
    e_try=30000 \
    debug=0 \
    mode=${mode}

date

# e_try=1,000,000 to get smooth curve
# e_try=10,000 for quick result


