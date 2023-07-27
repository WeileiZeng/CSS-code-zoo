MAKEFLAGS += --no-print-directory

# build options to test dynamic library
#LIB_WEILEI_PATH=/rhome/wzeng002/.local/lib
#LIB_WEILEI_PATH=/home/weileizeng/.local/lib
LIB_WEILEI_PATH=/sharedata01/weileizeng/.local/lib
LIB_WEILEI=-L$(LIB_WEILEI_PATH) -lweilei -Iweilei_lib


INC_DIR=weilei_lib
#INC_DIR=~/working/weilei_lib
CXX=g++ -O3 -Wall -std=c++11 -fopenmp
#CXX=g++ -O3 -Wall -std=c++11
# optimization options -O2 -O5 -Os
#ITPP=`pkg-config --cflags itpp` `pkg-config --libs itpp`
#ITPP=`itpp-config --cflags` `itpp-config --libs`
ITPP=-I/sharedata01/weileizeng/gitrepo/install-itpp-locally/itpp-4.3.1/include -O2 -DNDEBUG -L/sharedata01/weileizeng/gitrepo/install-itpp-locally/itpp-4.3.1/lib -litpp
# $(LIB_WEILEI)
#full command example
#g++ `pkg-config --cflags itpp` -o hello.out hello.cpp `pkg-config --libs itpp` -fopenmp


object_files=$(INC_DIR)/mm_read.o $(INC_DIR)/mmio.o $(INC_DIR)/mm_write.o $(INC_DIR)/lib.o $(INC_DIR)/dist.o $(INC_DIR)/product_lib.o $(INC_DIR)/bp.o 
header_files=$(INC_DIR)/mm_read.h $(INC_DIR)/mmio.h $(INC_DIR)/mm_write.h $(INC_DIR)/lib.h $(INC_DIR)/dist.h $(INC_DIR)/product_lib.h $(INC_DIR)/bp.h

#make object file for target file
%.o:%.cpp $(header_files)
#	$(CXX) $(START) $(END) -c $<
	$(CXX) $(ITPP) -c $<
#compile object files for lib files
lib:
	cd weilei_lib && make all
#now link object files

%.out:%.o $(object_files)
	$(CXX) $(ITPP) -o $@ $< $(object_files)

cmd=make lib && make $@.o && make $@.out # && ./$@.out
# eg: make test.o && make lib && make test.out
product:
	$(cmd)
test:
	$(cmd)
test_lib:
	$(cmd)
generate_css_code:
	$(cmd)
simulation:
	$(cmd)
verification:
	$(cmd)
reshape:
	$(cmd)
#if it keep running. clean and rebuild weilei_lib
#because I install it locally, i need to inform where the itpp lib is located
#	export LD_LIBRARY_PATH="/home/weileizeng/.local/lib:$LD_LIBRARY_PATH" && 
#This was moved to .bashrc
#	./$@.out

#add your new files here


clean:
	rm *.o
	rm *.out

#job management
sbatch-dry-run:
	sbatch --test run_prod.sh
sbatch:
	sbatch run_prod.sh
srun:
	srun -n 1 --cpus-per-task=32 --time=24:00:00 ./generate_css_code.out 
short:
	srun -n 1 -q short --cpus-per-task=32 --time=1:00:00 ./generate_css_code.out 
pkill-product:
	pkill .product


#test dynamic lib
dynamic:$(LIB_WEILEI_PATH)/libweilei.so
	cd weilei_lib && make libweilei.so
	$(CXX) $(ITPP) -o test_dynamic.out test.cpp -lweilei -L$(LIB_WEILEI_PATH)
	./test_dynamic.out


run-verification:
	srun --unbuffered -p small -n 1 --cpus-per-task=16 ./verification.out num_cores=16 code_folder="" filename_list=filelist-run3.txt
run-verification-full:
	srun --unbuffered -p small -n 1 --cpus-per-task=16 ./verification.out num_cores=16 code_folder="" filename_list=filelist-run3-full.txt
#	srun -p small -n 1 --cpus-per-task=16 --time=6:00:00 ./verification.out num_cores=16 filename_list=filelist-run2-full.txt
run-simulation:
	./run_simulation.sh
run-generate:
	sbatch sbatch_generate.sh

#steps to collect code date
#currently takes 1 minutes
#./get-filelist.sh
#./processing-codes.py
run_table:
	./get-filelist.sh
	python3 processing-codes.py
data-statistics:
	du -sh data/
	ls data/ |wc -l
show-new-codes:
	echo "total number of codes generated in this run"
	cat log/generate*.log |grep save|wc -l
show-table:
	tail -n 31 run.log



#The use of the -v flag in the command mounts the current working directory on the host (${PWD} in the example command) as /home/jovyan/work in the container. The server logs appear in the terminal.

#Visiting http://<hostname>:4000/?token=<token> in a browser loads JupyterLab.
jupyter:
	docker run -it --rm --user 1011 --group-add users -p 4001:8888 -e JUPYTER_TOKEN=passwd -v "${PWD}":/home/jovyan/work jupyter/datascience-notebook:85f615d5cafa


qos: 
	sacctmgr show qos
	sacctmgr show qos format=name%-10,Description%-20,priority,maxwall,UsageFactor,MaxTRESPU,MaxJobsPU,
	sacctmgr list qos format==name,priority,preempt

interactive:
	salloc -q long --time=1-00:00:00
