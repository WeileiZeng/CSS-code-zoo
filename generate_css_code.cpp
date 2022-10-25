#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include <ctime> //to get current time
using namespace common;

void test_mmio();
void test_getC();
void test_CSS_code();
void test_classical_code();
int generate_css_code();

int main(){
  std::cout<<" --------------------- begin test"<<std::endl;

  //  test_getC();
  //  test_classical_code();
  //  test_CSS_code();
  //test_mmio();
  generate_css_code();
  std::cout<<" --------------------- finish test"<<std::endl;
  return 0;
}




int generate_css_code(){

  // codeR.id_Gx=3511;
  // codeR.id_Gz=2657;
  //codeR.generate_by_id(0);
  //  itpp::RNG_randomize();
  //std::cout<<codeR<<std::endl;

  int seed = seed+get_time(3);
  itpp::RNG_reset(seed);
  itpp::RNG_randomize();

  int num_cores = 1; //32
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

    codeR.getGoodCode(0);
    if ( not codeR.is_valid() ){
      std::cout<<"The code is no valid"<<std::endl;
    }else{
      codeR.dist();
    }
    //the following part should be in critical
    //#pragma omp critical
    {
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

    //check if file exists. saev the code if not
    //set up filename
    std::string title_str="../data/CSSCode/run1/";
    int d =(codeR.dx < codeR.dz)? codeR.dx : codeR.dz ;
    int Gx_row_rank = codeR.Gx.row_rank(), Gz_row_rank = codeR.Gz.row_rank();
    codeR.k=codeR.n - Gx_row_rank - Gz_row_rank;
    std::string filename_prefix=title_str
      +"n"+std::to_string(codeR.n)+"k"+std::to_string(codeR.k)+"d"+std::to_string(d)
      +"-x"+std::to_string(Gx_row_rank)+"z"+std::to_string(Gz_row_rank)
      +"dx"+std::to_string(codeR.dx)+"dz"+std::to_string(codeR.dz);


    FILE *f;
    for (int j = 0; j++; j<4){//save three instances for the same parameter
      //filename_prefix = filename_prefix + "-"+std::to_string(j);
      std::string filename_Gx = filename_prefix + "-"+std::to_string(j) + "Gx.mm";
      const char* filename = filename_Gx.c_str();
      std::cout<<j<<std::endl;
      if ((f = fopen(filename, "r")) == NULL) {
	//save the code if file doesn't exist yet
	std::cout<<"saving code"<<filename<<std::endl;
	codeR.save(filename_prefix + "-" + std::to_string(j) );
	std::cout<<"saved code to "<<filename_prefix<<std::endl;
      }else{
	std::cout<<"the file exist: "<<filename_prefix<<std::endl;
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


// ============    implementations   ==============




void test_mmio(){
  itpp::GF2mat G = common::get_check_code743(7);
  GF2mat_to_MM(G, "tmp/G.mm");
  itpp::GF2mat H = MM_to_GF2mat("tmp/G.mm");
  std::cout<<H<<std::endl;
  return;
}

void test_getC(){
  std::cout<<"---------------------- begin test for getC()"<<std::endl;
  ClassicalCode code;
  code.get_743_code(7);
  code.dist();
  itpp::GF2mat H = code.H;
  std::cout<<"H"<<H<<std::endl;

  itpp::GF2mat G = H;//common::nullSpace(H);
  std::cout<<"G"<<G<<std::endl;
  if ( ( G*H.transpose() ).is_zero() ){
    std::cout<<"G*H^T=0\n";
  }
  itpp::GF2mat C = common::getC(G,H);
  std::cout<<"C"<<C<<std::endl;
  return;
}


void test_classical_code(){
  std::cout<<"begin test for ClassicalCode"<<std::endl;
  ClassicalCode code;
 
  if ( code.is_defined ){
    std::cout<<"code is_defined"<<std::endl;
  }else{
    std::cout<<"code is_defined"<<std::endl;
  }

  //Hamming code
  //  code.n = 7;
  code.get_743_code(7);
  code.full_rank();
  code.title="Hamming code";
  code.info();
  code.d = code.dist();
  std::cout<<"The Hamming [7,4,3] code has distance d="<<code.d<<std::endl;

  //repetition code
  code.title="repetition code";
  code.get_repetition_code(7);
  code.full_rank();
  //  code.info();
  code.d = code.dist();
  std::cout<<"repetition code: d="<<code.d<<std::endl;
  std::cout<<code<<std::endl;
  std::cout<<"finish test for ClassicalCode"<<std::endl;
  return;
}


void test_CSS_code(){
  std::cout<<"begin test for CSSCode"<<std::endl;
  CSSCode code;
  code.n = 7;
  code.title="Quantum hamming 713 code";
  code.get_713_code();
  //  std::cout<<code<<std::endl;
  //  code.full_rank();
  //  code.info();
  //  code.d = 
  code.dist();
  std::cout<<"[7,1,3] Hamming code: dx="<<code.dx<<std::endl;
  std::cout<<code<<std::endl;
  std::cout<<"finish test for CSSCode"<<std::endl;
  std::cout<<"begin test for ProductCSSCode"<<std::endl;

  //random CSS code
  CSSCode codeR;
  codeR.title="random code";
  codeR.n=7;
  codeR.Gx_row=3;
  codeR.Gz_row=3;
  codeR.id_Gx=3511;
  codeR.id_Gz=2657;
  codeR.generate_by_id(0);
  codeR.dist();

  std::cout<<codeR<<std::endl;
  std::cout<<codeR.Gx<<std::endl;
  std::cout<<codeR.Gz<<std::endl;


  ProductCSSCode codeP;
  codeP.n=10;
  codeP.title="some codeP";
  std::cout<<codeP<<std::endl;

    ConcatenatedProductCSSCode codeCP;
  codeCP.n=61;
  std::cout<<codeCP<<std::endl;


  SubsystemProductCSSCode codeSP;
  codeSP.n=9;
  SubsystemProductCSSCode codeSP2;
  codeSP=codeSP2;
  std::cout<<codeSP<<std::endl;
  //  codeSP.info();



  std::cout<<"finish test for ProductCSSCode"<<std::endl;
  return;
}

