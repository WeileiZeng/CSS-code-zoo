make simulation

# run case by case
case 23 in 
# check k=1
    13)
	code=n13k1d3-x6z6dx3dz3-9	
	code=n7k1d3-x3z3dx3dz3-0	
	;;
    14)
	code=n15k1d4-x8z6dx4dz4-0
	;;
    15)
#	code=n23k1d5-x11z11dx5dz5-0
	code=n25k1d5-x13z11dx5dz5-0  #run2
	;;
#    16)
#	code=n30k1d6-x15z14dx6dz6-0
#	;;
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



#code=steane1
output=${code}

mode=2  #0 for check; 1 for steane; 2 for simulating from file

#    code_prefix=../data/CSS-Codes/run2/n21k2d4-x10z9dx4dz4-5 \
#    code_prefix=../data/CSS-Codes/run1/${code} \
#   code_prefix=../data/CSS-Codes/run2/n24k1d5-x13z10dx5dz5-0 \

date 
srun --time=6:00:00 -p small -n 1 --cpus-per-task=16 \
    ./simulation.out \
    note=${code} \
    code_prefix=../data/CSS-Codes/run2/${code} \
    output=result/${output}.json \
    num_cores=16 \
    e_try=100000 \
    debug=0 \
    mode=${mode}

date

# e_try=1,000,000 to get smooth curve
# e_try=10,000 for quick result


