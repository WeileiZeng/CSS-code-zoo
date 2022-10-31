#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include "json.hpp"
using json=nlohmann::json;
//#include <ctime> //to get current time
//using namespace common;

int generate_css_code();

int main(){
  std::cout<<" --------------------- begin generating CSS codes"<<std::endl;
  generate_css_code();
  std::cout<<" --------------------- finish generating CSS codes"<<std::endl;
  return 0;
}


int generate_css_code(){

  int seed = seed+common::get_time(3);
  itpp::RNG_reset(seed);
  itpp::RNG_randomize();

  int num_cores = 16; //32
  int num_trials = num_cores * 10000;
  int dx_max=0, dz_max=0;
  //#pragma omp parallel for num_threads(4)
#pragma omp parallel for schedule(guided) num_threads(num_cores)
  for ( int i =0; i < num_trials; i ++ ){
    //random CSS code
    CSSCode codeR;
    codeR.title="random code";
    // good choice: 15,6,6;
    codeR.n=22;
    codeR.Gx_row=15;
    codeR.Gz_row=5;

    codeR.getGoodCode(0);//0 for disabling debug
    codeR.dist();
    /* if ( not codeR.is_valid() ){
      std::cout<<"The code is not valid"<<std::endl;
    }else{
      codeR.dist();
      }*/
    //the following part should be in critical in the beginning, after some run it is okay to remove
//#pragma omp critical
    {
      /*
    if (dx_max <= codeR.dx)  {
      dx_max = codeR.dx;
      codeR.k=codeR.n - codeR.Gx.row_rank() - codeR.Gz.row_rank();
      std::cout<<"dx_max = "<<dx_max<<", dz_max = "<<dz_max;
      std::cout<<"; "<<codeR<<std::endl;
    }
    if (dz_max <= codeR.dz)  {
      dz_max = codeR.dz ;
      codeR.k=codeR.n - codeR.Gx.row_rank() - codeR.Gz.row_rank();
      std::cout<<"dx_max = "<<dx_max<<", dz_max = "<<dz_max;
      std::cout<<"; "<<codeR<<std::endl;
    }
      */
    //check if file exists. saev the code if not
    //set up filename
    std::string title_str="../data/CSS-Codes/run1/";
    int d =(codeR.dx < codeR.dz)? codeR.dx : codeR.dz ;
    int Gx_row_rank = codeR.Gx.row_rank(), Gz_row_rank = codeR.Gz.row_rank();
    codeR.k=codeR.n - Gx_row_rank - Gz_row_rank;
    std::string filename_prefix=title_str
      +"n"+std::to_string(codeR.n)+"k"+std::to_string(codeR.k)+"d"+std::to_string(d)
      +"-x"+std::to_string(Gx_row_rank)+"z"+std::to_string(Gz_row_rank)
      +"dx"+std::to_string(codeR.dx)+"dz"+std::to_string(codeR.dz);

    FILE *f;
    for (int j = 1; j<4; j++){//save three instances for the same parameter
      //      std::cout<<"debug:1"<<std::endl;
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
	//std::cout<<"saving code: "<<filename<<std::endl;
	//codeR.save(filename_prefix + "-" + std::to_string(j) );
	//itpp::GF2mat Gt = codeR.Gz;
	//	std::cout<<filename_Gz<<std::endl;
	GF2mat_to_MM(codeR.Gz,filename_Gx);
	GF2mat_to_MM(codeR.Gz,filename_Gz);
	//	GF2mat_to_MM(codeR.Gz,filename_
	std::cout<<"saved code to "<<filename_json<<std::endl;
	break;
      }else{
	//std::cout<<"the file exist: "<<filename_prefix<<std::endl;
	fclose(f);
      }
    }
    
    }
    //    std::cout<<codeR.Gx<<std::endl;
    //std::cout<<"run"<<i<<", dx="<<codeR.dx<<std::endl;
  }
  std::cout<<"dx_max = "<<dx_max<<", dz_max = "<<dz_max<<std::endl;
  
  //  codeR.set_up_CxCz(); // no need to do it. already genrated in GetRandomCode().
  

  /*std::cout<<codeR.is_C_defined<<std::endl;
  std::cout<<codeR.Gx<<std::endl;
  std::cout<<codeR.Gz<<std::endl;
  std::cout<<codeR.Cx<<std::endl;
  std::cout<<codeR.Cz<<std::endl;
  */
  return 0;
}



