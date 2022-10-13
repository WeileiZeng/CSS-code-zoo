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
//random CSS code
  CSSCode codeR;
  codeR.title="random code";
  codeR.n=20;
  codeR.Gx_row=6;
  codeR.Gz_row=6;
  // codeR.id_Gx=3511;
  // codeR.id_Gz=2657;
  //codeR.generate_by_id(0);
  //  itpp::RNG_randomize();
  //std::cout<<codeR<<std::endl;

  int seed = seed+get_time(3);
  itpp::RNG_reset(seed);
  itpp::RNG_randomize();

  int num_cores = 32; //32
  int num_trials = num_cores * 10000;
  int dx_max=0, dz_max=0;
  //#pragma omp parallel for num_threads(4)
#pragma omp parallel for schedule(guided) num_threads(num_cores)
  for ( int i =0; i < num_trials; i ++ ){
    codeR.getGoodCode(0);
    codeR.dist();
    //the following part should be in critical
#pragma omp critical
    {
    if (dx_max < codeR.dx)  {
      dx_max = codeR.dx;
      std::cout<<"dx_max = "<<dx_max<<", dz_max = "<<dz_max<<std::endl;
    }
    if (dz_max < codeR.dx)  {
      dz_max = codeR.dz ;
      std::cout<<"dx_max = "<<dx_max<<", dz_max = "<<dz_max<<std::endl;
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

