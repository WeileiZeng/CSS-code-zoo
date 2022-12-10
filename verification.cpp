#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include "json.hpp"
using json=nlohmann::json;
#include <chrono> //print computation time
#include <ctime>  

/* Find the code according to the filename, calculate distance and check if it matched value saved in json file
 *@param code_prefix, code_prefix=code_folder+code_name
 *@return bool, true if match, otherwise false.
 */
bool verify(std::string code_prefix){
	CSSCode code;
	code.load(code_prefix);
	code.dist();
	std::ifstream jsonfile(code_prefix+".json");
	json data = json::parse(jsonfile);
	jsonfile.close();
	return code.d == data["d"];
	//	std::cout<<data<<std::endl;
	//	std::cout<<"saved distance     d="<<data["d"]<<std::endl;
	//	std::cout<<"caculated distance d="<<code.d<<std::endl;

}

/* Counting number of lines in a file. same as `wc -l filename`
 */
int count_lines(std::string filename){
  int code_total = 0;
  std::string line;
  std::ifstream file(filename);
  if (file.is_open()) {
    while (std::getline(file, line))
      code_total++;
  }
  file.close();
  return code_total;
}

/** Verify data in CSS code zoo. Calculate the distance and make sure it matches the value saved in json file. Final result will be printed.
 *@param debug
 *@param filename_list, the text file save filenames corresponding to the codes.
 *@param code_folder, the folder save json file and matriices of the codes.
 *@param num_cores, number of cores used in openmp.
 */
int main(int args, char ** argv){
    std::cout<<"============begin verification========="<<std::endl;
    itpp::Parser parser;parser.init(args,argv);
    //    parser.set_silentmode(true);
    std::string filename_list="filelist-run2.txt";
    parser.get(filename_list,"filename_list");
    std::string code_folder="../data/CSS-Codes/run2/";
    parser.get(code_folder,"code_folder");
    int num_cores=4; parser.get(num_cores,"num_cores");

    //get number of codes
    const int code_total=count_lines(filename_list);
    std::cout<<"Number of codes: "<<code_total<<std::endl;
    std::cout<<"No printing on srun due to slow network"<<std::endl;
    int code_count=0,code_mistake=0;
    std::ifstream file(filename_list);
    if (file.is_open()) {
#pragma omp parallel for schedule(guided) num_threads(num_cores)
      for ( int i=0; i<code_total; i++){
	std::string jsonfile_name,code_name;
#pragma omp critical
	{
	  if (code_count % 1000 == 0) { 
	    //printing on nodes are very slow
	    printf("%i codes counted, %i mistakes.\n",code_count,code_mistake);
	  }
	  std::getline(file, jsonfile_name);
	}
	code_name = jsonfile_name.substr(0, jsonfile_name.size()-5);
	//        printf("%s", line.c_str());
	//	printf("%s", code_name.c_str());
	code_count++;
	if (verify(code_folder+code_name)){
	  //	  std::cout<<"equal"<<std::endl;
	}else{
	  code_mistake++;
	  //  std::cout<<code_name<<":\td = "<<data["d"]
	  //	   <<", code.d = " <<code.d<<std::endl;
	}
	//	break;

      }//pragma for
      file.close();
    }//if
    std::cout<<code_count<<" codes checked, "
	     <<code_mistake<<" mistakes found"<<std::endl;
    return 0;



}

