make simulation


case 25 in 
# check k=1
    13)
	code=n13k1d3-x6z6dx3dz3-9
	;;
    14)
	code=n15k1d4-x7z7dx4dz4-0
	;;
    15)
	code=n23k1d5-x11z11dx5dz5-0
	;;
#now for k=2, choose min n for that k,d
    23)
# for 0,1,2, the distance is not 3. need to check
# also need to fix Gx Gz to be full rank
#	code=n11k2d3-x4z5dx3dz3-0,1,3,
	code=n11k2d3-x4z5dx3dz3-2
	;;
    24)
	code=n18k2d4-x8z8dx4dz4-4
#	code=n17k2d4-x7z8dx4dz4-0 #d=3
	;;
    25)
	#can not find code with d=5, k=2
	code=n26k2d5-x12z12dx6dz5-0
	;;
esac

output=${code}

mode=0  #0 for check; 1 for steane; 2 for simulating from file

date 
srun --time=24:00:00 ./simulation.out \
    note=${code} \
    code_prefix=../data/CSS-Codes/run1/${code} \
    output=result/${output}.json \
    num_cores=32 \
    e_try=10000 \
    debug=0 \
    mode=${mode}

date

# e_try=1,000,000 to get smooth curve
# e_try=10,000 for quick result


