make simulation

# time ./simulation.out num_cores=32 debug=1 e_try=1000 output=tmp.json note="I am happy"

code=n8k1d3-x4z3dx3dz3-0
code=n7k1d3-x3z3dx3dz3-0
output=steane2.json


code=n11k1d3-x5z5dx3dz3-0
code=n12k1d3-x6z5dx3dz3-9



case 3 in 
    3)
	code=n13k1d3-x6z6dx3dz3-9
	output=d3.json
	;;
    4)
	code=n15k1d4-x7z7dx4dz4-0
	output=d4.json
	;;
    5)
	code=n23k1d5-x11z11dx5dz5-0
	output=d5.json
	;;

esac

./simulation.out code_prefix=../data/CSS-Codes/run1/${code} \
output=result/${output} num_cores=16 e_try=100000 debug=0


date
