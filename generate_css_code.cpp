#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include "json.hpp"
using json=nlohmann::json;
#include <chrono> //print computation time
#include <ctime>  


/*generate random CSS code and save.
 *@param n, number of qubits
 *@param Gx_row, number of rows of Gx, not necessarily equal to rank
 *@param Gz_row,
 *@param num_cores, 32
 *@param code_folder, folder to save the code
 *@param num_trials, 10000
  parameters to set up:
  @param how many instances to save for each parameter set: 10
  data folder, ../data/CSS-Codes/run1
 *@return num_code_generated
 */
int generate_css_code(int n, int Gx_row, int Gz_row, int num_cores, std::string code_folder, int num_trials);

int main(int args, char ** argv){
  std::cout<<" --------------------- begin generating CSS codes"<<std::endl;
  itpp::Parser parser; parser.init(args,argv);
  int n_start=5; parser.get(n_start,"n_start");
  int n_end=10; parser.get(n_end,"n_end");
  int num_cores=1; parser.get(num_cores,"num_cores");
  std::string code_folder="../data/CSS-Codes/tmp/";
  parser.get(code_folder,"code_folder");
  int num_trials=num_cores*2;parser.get(num_trials,"num_trials");
  int num_code_generated_total=0;
  std::string note="no note";parser.get(note,"note");
  //set up simulation
  for (int n=n_start;n<n_end;n++){
    for ( int Gx_row = 1;Gx_row<n-1;Gx_row++){
      //	std::cout<<"n="<<n<<", Gx_row="<<Gx_row<<std::endl;
      //      for ( int Gz_row = (n-Gx_row-3>2)?n-Gx_row-3:2 ; Gz_row < n-Gx_row && Gz_row < Gx_row +3 ; Gz_row ++){ //run for k > 22
      for ( int Gz_row = 1; Gz_row < n-Gx_row && Gz_row < Gx_row +3 ; Gz_row ++){ //run for k > 22
	//	if (Gx_row > 3 || Gz_row >3) continue;	

	std::cout<<"n="<<n<<", Gx_row="<<Gx_row<<", Gz_row="<<Gz_row<<std::endl;

	auto start = std::chrono::system_clock::now();
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout <<".-.-.-.-.-.-.-.-.-.-.-."
		  << "start computation at " << std::ctime(&start_time)<<std::endl;
	num_code_generated_total += generate_css_code(n,Gx_row,Gz_row,num_cores,code_folder,num_trials);
	//When G_x_row or Gz_row is too small (like 1), error occurs, probably get zero matrix somewhere

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "Elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
      }
    }
  }
  std::cout<<num_code_generated_total<<" codes generated this time for n_start = "<<n_start<<", n_end = "<<n_end<<std::endl;
  std::cout<<" --------------------- finish generating CSS codes"<<std::endl;
  return 0;
}


int generate_css_code(int n, int Gx_row, int Gz_row, int num_cores, std::string code_folder, int num_trials){
  int seed = common::get_time(3)+100;//+seed;
  itpp::RNG_reset(seed);
  itpp::RNG_randomize();
  int num_code_generated=0; //counting number of codes generated during this run, and print it in the end.
  //  int dx_max=0, dz_max=0;
#pragma omp parallel for schedule(guided) num_threads(num_cores)
  for ( int i =0; i < num_trials; i ++ ){
    //generate random CSS code
    CSSCode codeR;
    codeR.title="Random code";
    codeR.n=n;
    codeR.Gx_row=Gx_row;
    codeR.Gz_row=Gz_row;

    codeR.getGoodCode(0);//0 for disabling debug
    codeR.dist();
    //the following part should be in critical in the beginning, after some run it is okay to remove
#pragma omp critical
    {
      //check if file exists. save the code if not
      //set up filename with necessary parameters
      int d =(codeR.dx < codeR.dz)? codeR.dx : codeR.dz ;
      int Gx_row_rank = codeR.Gx.row_rank(), Gz_row_rank = codeR.Gz.row_rank();
      codeR.k=codeR.n - Gx_row_rank - Gz_row_rank;
      //add folder
      std::string filename_prefix=
	+"n"+std::to_string(codeR.n)+"/k"+std::to_string(codeR.k)+"/"
	+"n"+std::to_string(codeR.n)+"k"+std::to_string(codeR.k)+"d"+std::to_string(d)
	+"-x"+std::to_string(Gx_row_rank)+"z"+std::to_string(Gz_row_rank)
	+"dx"+std::to_string(codeR.dx)+"dz"+std::to_string(codeR.dz);
      /*      std::string filename_prefix=code_folder
	+"n"+std::to_string(codeR.n)+"k"+std::to_string(codeR.k)+"d"+std::to_string(d)
	+"-x"+std::to_string(Gx_row_rank)+"z"+std::to_string(Gz_row_rank)
	+"dx"+std::to_string(codeR.dx)+"dz"+std::to_string(codeR.dz);
      */
      FILE *f;
      for (int j = 0; j<10; j++){//save 10 instances for the same parameter
	char filename_Gx[256],filename_Gz[256],filename_json[256];
	/*sprintf(filename_Gx,"%s-%iGx.mm",filename_prefix.c_str(),j);
	sprintf(filename_Gz,"%s-%iGz.mm",filename_prefix.c_str(),j);
	sprintf(filename_json,"%s-%i.json",filename_prefix.c_str(),j);*/
	sprintf(filename_Gx,"%s/i%i/%s-%iGx.mm",code_folder.c_str(),j,filename_prefix.c_str(),j);
	sprintf(filename_Gz,"%s/i%i/%s-%iGz.mm",code_folder.c_str(),j,filename_prefix.c_str(),j);
	sprintf(filename_json,"%s/i%i/%s-%i.json",code_folder.c_str(),j,filename_prefix.c_str(),j);

	const char* filename = filename_Gx;
	if ((f = fopen(filename, "r")) == NULL) {
	  //save the code if file doesn't exist yet
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
	  std::cout<<"Saved code to "<<filename_json<<std::endl;
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



