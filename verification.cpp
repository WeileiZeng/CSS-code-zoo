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
	code.Gx.density();
	std::ifstream jsonfile(code_prefix+".json");
	json data = json::parse(jsonfile);
	jsonfile.close();
	return code.d == data["d"];
	//	std::cout<<data<<std::endl;
	//	std::cout<<"saved distance     d="<<data["d"]<<std::endl;
	//	std::cout<<"caculated distance d="<<code.d<<std::endl;

}

bool verify(std::string code_prefix, double & rho_x, double & rho_z, int & n_Gx, int & n_Gz){
	CSSCode code;
	code.load(code_prefix);

	rho_x = code.Gx.density();
	rho_z = code.Gz.density();
	n_Gx = code.Gx.rows()*code.Gx.cols();
	n_Gz = code.Gz.rows()*code.Gz.cols();

	code.dist();
	std::ifstream jsonfile(code_prefix+".json");
	json data = json::parse(jsonfile);
	jsonfile.close();
	return code.d == data["d"];		
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

    //get number of codes in advance to set up parallel for
    const int code_total=count_lines(filename_list);
    //    const int code_total=1000;
    std::cout<<"Number of codes: "<<code_total<<std::endl;
    //std::cout<<"No printing on srun due to slow network"<<std::endl; //fixed by setting srun --unbuffered
    int code_count=0,code_mistake=0;
    double rho_x_total=0,rho_z_total=0;
    int n_Gx_total=0,n_Gz_total=0;
    std::ifstream file(filename_list);
    if (file.is_open()) {
#pragma omp parallel for schedule(guided) num_threads(num_cores)
      for ( int i=0; i<code_total; i++){
	std::string jsonfile_name,code_name;
#pragma omp critical
	{
	code_count++;
	if (code_count % 1000 == 0){
	  //printing on nodes are very slow
	  printf("%i/%i codes counted, %i mistakes.\n",code_count,code_total, code_mistake);
	  std::cout<<"Gx density:"<<rho_x_total /n_Gx_total
	     <<",\tGz density:"<<rho_z_total /n_Gz_total
	     <<std::endl;
	}
	std::getline(file, jsonfile_name);
      }//critical
	code_name = jsonfile_name.substr(0, jsonfile_name.size()-5);
	//        printf("%s", line.c_str());
	//	printf("%s", code_name.c_str());
	//	if (verify(code_folder+code_name)){
	double rho_x,rho_z;
	int n_Gx,n_Gz;
	if (verify(code_folder+code_name, rho_x, rho_z, n_Gx, n_Gz)){
	  //	  std::cout<<"equal"<<std::endl;
	}else{
	  code_mistake++;
	  //  std::cout<<code_name<<":\td = "<<data["d"]
	  //	   <<", code.d = " <<code.d<<std::endl;
	}
#pragma omp critical
	{
	rho_x_total += rho_x*n_Gx;
	rho_z_total += rho_z*n_Gz;
	n_Gx_total += n_Gx;
	n_Gz_total += n_Gz;
	}//critical
      }//pragma for
      file.close();
    }//    if (file.is_open()) 

    std::cout<<"Gx density:"<<rho_x_total /n_Gx_total
	     <<",\tGz density:"<<rho_z_total /n_Gz_total
	     <<std::endl;
    std::cout<<code_count<<" codes checked, "
	     <<code_mistake<<" mistakes found"<<std::endl;
    return 0;



}

