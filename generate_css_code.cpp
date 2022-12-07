#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include "json.hpp"
using json=nlohmann::json;
#include <chrono> //print computation time
#include <ctime>  
//#include <ctime> //to get current time
//using namespace common;

//int generate_css_code();
int generate_css_code(int n, int Gx_row, int Gz_row, int num_cores, std::string code_folder, int num_trials);

int main(int args, char ** argv){
  std::cout<<" --------------------- begin generating CSS codes"<<std::endl;
  itpp::Parser parser; parser.init(args,argv);
  int n_start=5; parser.get(n_start,"n_start");
  int n_end=10; parser.get(n_end,"n_end");
  int num_cores=1; parser.get(num_cores,"num_cores");
  std::string code_folder="../data/CSS-Codes/tmp/";
  parser.get(code_folder,"code_folder");
  int num_trials=num_cores*2;parser.get(num_cores,"num_cores");
  int num_code_generated_total=0;
  //set up simulation
  for (int n=n_start;n<n_end;n++){
    for ( int Gx_row = 2;Gx_row<n-1;Gx_row++){
      //	std::cout<<"n="<<n<<", Gx_row="<<Gx_row<<std::endl;
      for ( int Gz_row = 2 ; Gz_row < n-Gx_row && Gz_row < Gx_row +3 ; Gz_row ++){ //run for k > 22
	std::cout<<"n="<<n<<", Gx_row="<<Gx_row<<", Gz_row="<<Gz_row<<std::endl;

	auto start = std::chrono::system_clock::now();
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout <<".-.-.-.-.-.-.-.-.-.-.-."
		  << "start computation at " << std::ctime(&start_time)<<std::endl;
	num_code_generated_total += generate_css_code(n,Gx_row,Gz_row,num_cores,code_folder,num_trials);
	//When G_x_row or Gz_row is too small (like 1), error occurs, probably get zero matrix somewhere

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s"
		  << std::endl;
      }
    }
  }
  std::cout<<num_code_generated_total<<" codes generated this time for n_start = "<<n_start<<std::endl;
  std::cout<<" --------------------- finish generating CSS codes"<<std::endl;
  return 0;
}

/*parameters to set up:
  @param num_cores,32
  @param num_trials, 10000
  @param how many instances to save for each parameter set, 3
  data folder, ../data/CSS-Codes/run1
  @return num_code_generated
 */
int generate_css_code(int n, int Gx_row, int Gz_row, int num_cores, std::string code_folder, int num_trials){
  //int generate_css_code(){
  int seed = common::get_time(3)+100;//+seed;
  itpp::RNG_reset(seed);
  itpp::RNG_randomize();
  int num_code_generated=0;

  //  int num_cores = 16; //32
  //  int num_trials = num_cores * 10;
  int dx_max=0, dz_max=0;
#pragma omp parallel for schedule(guided) num_threads(num_cores)
  for ( int i =0; i < num_trials; i ++ ){
    //random CSS code
    CSSCode codeR;
    codeR.title="random code";
    // good choice: 15,6,6;
    codeR.n=n;//22;
    codeR.Gx_row=Gx_row;//15;
    codeR.Gz_row=Gz_row;//5;

    codeR.getGoodCode(0);//0 for disabling debug
    codeR.dist();
    //the following part should be in critical in the beginning, after some run it is okay to remove
#pragma omp critical
    {
      //check if file exists. save the code if not
    //set up filename
      //    std::string title_str="../data/CSS-Codes/run2/";
    int d =(codeR.dx < codeR.dz)? codeR.dx : codeR.dz ;
    int Gx_row_rank = codeR.Gx.row_rank(), Gz_row_rank = codeR.Gz.row_rank();
    codeR.k=codeR.n - Gx_row_rank - Gz_row_rank;
    std::string filename_prefix=code_folder
      +"n"+std::to_string(codeR.n)+"k"+std::to_string(codeR.k)+"d"+std::to_string(d)
      +"-x"+std::to_string(Gx_row_rank)+"z"+std::to_string(Gz_row_rank)
      +"dx"+std::to_string(codeR.dx)+"dz"+std::to_string(codeR.dz);

    FILE *f;
    for (int j = 0; j<10; j++){//save 10 instances for the same parameter
      //filename_prefix = filename_prefix + "-"+std::to_string(j);
      char filename_Gx[256],filename_Gz[256],filename_json[256];
      sprintf(filename_Gx,"%s-%iGx.mm",filename_prefix.c_str(),j);
      sprintf(filename_Gz,"%s-%iGz.mm",filename_prefix.c_str(),j);
      sprintf(filename_json,"%s-%i.json",filename_prefix.c_str(),j);

      //      std::string filename_Gx = filename_prefix + "-"+std::to_string(j) + "Gx.mm";
      //std::string filename_json = filename_prefix + "-"+std::to_string(j) + ".json";
      //      const char* filename = filename_Gx.c_str();
      const char* filename = filename_Gx;
      //std::cout<<j<<std::endl;
      if ((f = fopen(filename, "r")) == NULL) {
	//save the code if file doesn't exist yet

	/*don't check here. break the parallel set up
	//check 10 times before add the code
	int d=codeR.d;
	for (int i_check=0;i_check < 10; i_check ++){
	  codeR.dist();
	  if ( codeR.d < d){
	    //discard the code
	    std::cout<<"d="<<d<<", code.d="<<codeR.d<<std::endl;
	    continue;
	  }else{
	    std::cout<<".";
	  }
	}
	*/
	GF2mat_to_MM(codeR.Gx,filename_Gx);
	GF2mat_to_MM(codeR.Gz,filename_Gz);

	//save parameters into json file
	json::object_t object_value = {
	  {"n",codeR.n},
	  {"k",codeR.k},
	  {"d",d},
	  {"Gx_row",Gx_row_rank},
	  {"Gz_row",Gz_row_rank},
	  {"dx",codeR.dx},
	  {"dz",codeR.dz},
	  {"index",j},
	  {"filename_Gx",filename_Gx},
	  {"filename_Gz",filename_Gz},
	};
	json j_object_t(object_value);
	//	std::cout<<j_object_t<<std::endl;
	std::ofstream filejson(filename_json);
	filejson << j_object_t;
	filejson.close();
	
	num_code_generated++;
	std::cout<<"saved code to "<<filename_json<<std::endl;
	break;
      }else{
	//std::cout<<"the file exist: "<<filename_prefix<<std::endl;
	fclose(f);
      }
    }//for j
    
    }//pragma critical
  }// pragma parallel for
  
  return num_code_generated;
}



