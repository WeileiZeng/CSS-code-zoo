make reshape
#define code<k>, code<k>[<d>]
code1=(NA NA NA n7k1d3-x3z3dx3dz3-0 n15k1d4-x8z6dx4dz4-0 n25k1d5-x13z11dx5dz5-0)
code2=(NA NA NA n11k2d3-x4z5dx3dz3-0 n18k2d4-x8z8dx4dz4-0 n27k2d5-x13z12dx5dz5-0)
code3=(NA NA NA n11k3d3-x4z4dx3dz3-0 n19k3d4-x9z7dx4dz4-0 n29k3d5-x13z13dx5dz5-0)


echo "The seed codes used to construct SPC: code<k>[<d>]"
for i in {3..5}; do echo code1$i, ${code1[$i]}; done;
for i in {3..5}; do echo code2$i, ${code2[$i]}; done;


# run case by case, kd, kdkd
case 2525 in 
    1313)
	codeA=n7k1d3-x3z3dx3dz3-0	
	codeB=n7k1d3-x3z3dx3dz3-0	
	;;
    1314)
	codeA=n7k1d3-x3z3dx3dz3-0	
	codeB=n15k1d4-x8z6dx4dz4-0
	;;
    1315)
	codeA=n7k1d3-x3z3dx3dz3-0
	codeB=n25k1d5-x13z11dx5dz5-0
	;;	
    1515)
	codeA=n25k1d5-x13z11dx5dz5-0
	codeB=n25k1d5-x13z11dx5dz5-0
	;;
    2525)
	codeA=n27k2d5-x13z12dx5dz5-0
	codeB=n27k2d5-x13z12dx5dz5-0
	;;
	
# check k=1
    13)
	code=n7k1d3-x3z3dx3dz3-0	
	;;
    14)
	code=n15k1d4-x8z6dx4dz4-0
	;;
    15)
	code=n25k1d5-x13z11dx5dz5-0
	;;
#now for k=2, choose min n for that k,d
    23)
	code=n11k2d3-x4z5dx3dz3-0
	;;
    24)
	code=n18k2d4-x8z8dx4dz4-0
	;;
    25)
	code=n27k2d5-x13z12dx5dz5-0
	;;
esac

codeA=${code1[5]}
codeB=${code1[5]}


#code=steane1
output=${codeA}_${codeB}
echo ${output}

mode=2  #0 for check; 1 for steane; 2 for simulating from file

#    code_prefix=../data/CSS-Codes/run2/n21k2d4-x10z9dx4dz4-5 \
#    code_prefix=../data/CSS-Codes/run1/${code} \
#   code_prefix=../data/CSS-Codes/run2/n24k1d5-x13z10dx5dz5-0 \

date 

# check steane codes.
#srun -p small ./reshape.out mode=1 e_try=10000 num_cores=16
#srun ./reshape.out mode=1 e_try=1000000 num_cores=32
#exit

#codeA
for i in {3..5}
do
    #codeB
    for j in {3..5}
    do
	if [ $i -le $j ]
	then
	    codeA=${code2[$i]}
	    codeB=${code2[$j]}
	    title=2${i}2${j}
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
    e_try=10000 \
    debug=0 \
    mode=${mode} &
	    wait
	    exit
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


