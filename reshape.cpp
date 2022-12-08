#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include "json.hpp"
using json=nlohmann::json;
#include <chrono> //print computation time
#include <ctime>  
//#include <ctime> //to get current time
//using namespace common;

//return a long vector consisting of rows of G
itpp::bvec GF2mat_flat(itpp::GF2mat G){
  itpp::GF2mat flat_G(G.get_row(0), false);//false for row
  //  std::cout<<"G"<<G<<std::endl;
  for ( int i=1 ; i<G.rows();i++){
    //    std::cout<<"i="<<i<<","<<flat_G<<std::endl;
    flat_G=flat_G.concatenate_horizontal(itpp::GF2mat(G.get_row(i),false));
    //    std::cout<<"i="<<i<<","<<flat_G<<std::endl;
  }
  //  std::cout<<flat_G<<std::endl;
  return flat_G.bvecify();				    
}

void syndrome_print(itpp::bvec syndrome, SubsystemProductCSSCode spc){
  itpp::GF2mat syndrome_A(spc.codeA.Gx.rows(),spc.codeB.n),
    syndrome_B(spc.codeA.n,spc.codeB.Gx.rows());
  //  std::cout<<"syndrome_A:"<<syndrome_A<<std::endl;
  for ( int i=0; i<spc.codeA.Gx.rows(); i++ ){
    //    itpp::bvec b = syndrome[1,3];
    //    std::cout<<"row" <<syndrome(i*spc.codeB.n,(i+1)*spc.codeB.n-1) <<std::endl;
    syndrome_A.set_row(i,syndrome(i*spc.codeB.n,(i+1)*spc.codeB.n-1));
  }
  std::cout<<"syndrome_A:"<<syndrome_A<<std::endl;

  int sizeA=syndrome_A.rows()*syndrome_A.cols();
  for ( int i=0; i<spc.codeA.n; i++){
    syndrome_B.set_row(i,syndrome(sizeA+i*spc.codeB.Gx.rows(), sizeA+(i+1)*spc.codeB.Gx.rows()-1));
  }
  std::cout<<"syndrome_B:"<<syndrome_B<<std::endl;

}

bool reshape_decode(SubsystemProductCSSCode spc, itpp::GF2mat e){

  return false;
}

int simulate(double p){
  //  spc.codeA;
  //  spc.codeB.Gx;
  /*take twos codes, say Steane codes. construt SPC, generate error e, then decode it.
   */

  CSSCode codeA,codeB;
  codeA.n = 7; codeA.title="Steane 713 code"; codeA.get_713_code();
  codeB.n = 7; codeB.title="Steane 713 code"; codeB.get_713_code();
  codeA.Gx=common::make_it_full_rank(codeA.Gx);
  codeB.Gx=common::make_it_full_rank(codeB.Gx);
  codeA.Gz=common::make_it_full_rank(codeA.Gz);
  codeB.Gz=common::make_it_full_rank(codeB.Gz);
  codeA.set_up_CxCz();
  codeB.set_up_CxCz();
  //  std::cout<<codeA.Cx<<std::endl;

  SubsystemProductCSSCode spc(codeA,codeB);
  spc.product();
  spc.n=spc.codeA.n*spc.codeB.n;

  //  std::cout<<spc.Gx<<std::endl;//size 98*49

  //set up random error
  itpp::GF2mat e_input(spc.codeA.n,spc.codeB.n);
  //each row corresponding to code A, each column corresponding to codes B. e_vector=[e_input.get_row(_) for _ in 0...codeB.n-1]
  p=9.0/spc.n;
  for ( int i=0; i<e_input.rows(); i++) 
    for ( int j=0; j<e_input.cols(); j++) 
      e_input.set(i,j,(itpp::randu()-p<0)? 1:0);
  //  e_input.set(2,2,1);

  std::cout<<"spc.codeA.Gx"<<spc.codeA.Gx
	   <<"spc.codeB.Gx"<<spc.codeB.Gx<<std::endl;

  //  common::GF2matPrint(e_input,"e_input");
  std::cout<<"e_input"<<e_input<<std::endl;
  //caluclate syndrome
  itpp::bvec e_vector = GF2mat_flat(e_input);
  //  std::cout<<"e_vector: "<<e_vector<<std::endl;
  itpp::bvec syndrome = spc.Gx*e_vector;
  //  std::cout<<"syndrome: "<<syndrome<<std::endl;
  //  syndrome_print(syndrome, spc);


  //decode by col, for code A
  itpp::GF2mat e_decoded(e_input);
  for ( int i =0 ; i<e_input.cols(); i++){
    itpp::bvec e_col = e_input.get_col(i);
    std::cout<<e_col;
    bool ans=codeA.decode(e_col,100,0) ;
    std::cout<<e_col<<ans<<std::endl;
    e_decoded.set_col(i,e_col);
  }
  e_input=e_input+e_decoded;
  std::cout<<"After decoding by col, code A, e_input"<<e_input<<std::endl;
  //decode by row
  for ( int i =0 ; i<e_input.rows(); i++){
    itpp::bvec e_row = e_input.get_row(i);
    std::cout<<e_row;
    bool ans=codeB.decode(e_row,100,0) ;
    std::cout<<e_row<<ans<<std::endl;
    e_decoded.set_row(i,e_row);
  }
  e_input=e_input+e_decoded;
  std::cout<<"After decoding by row, code B, e_input"<<e_input<<std::endl;



    //    std::cout<<e_col<<std::endl;
  return 0;
}

/** Decoding simulation for given CSS code using random window decoder
 *@param num_cores
 *@param note
 *@param debug 
 *@param output_json save simulation result
 *@param e_try number of errors to be simulated for each data point
 *@param code_prefix prefix to find the code
 *@param mode 0 for check; 1 for Steane; 2 for reading code file
 *@return None. simulateion result saved in json file.
 */
int main(int args, char ** argv){
  
  itpp::RNG_randomize();
  simulate(0.1);
  return 0;
  
    std::cout<<"============begin simulation========="<<std::endl;
    //parse parameter
    itpp::Parser parser;parser.init(args,argv);
    //    parser.set_silentmode(true);
    int num_cores=1; parser.get(num_cores,"num_cores");
    std::string note="no-note"; parser.get(note,"note");
    int debug=1; parser.get(debug,"debug");//default debug on
    std::string output_json ="tmp.json"; parser.get(output_json,"output");
    int e_try=100; parser.get(e_try,"e_try");
    std::string code_prefix="NA";
    parser.get(code_prefix,"code_prefix");
    int mode = 3; parser.get(mode,"mode");

    //initialize code
    CSSCode code;
    switch (mode){
    case 0://check mode. print information and quit
      std::cout<<"Printing code info:"<<std::endl;
      code.load(code_prefix);
      code.title=note;
      code.dist();
      code.k = code.n - code.Gx.row_rank() - code.Gz.row_rank();
      std::cout<<code<<std::endl;
      //      code.info();
      std::cout<<"Finish checking code; exit the program; no simulation has been ran."<<std::endl;
      return 0;
      break;
    case 1: //Steane code      
      code.n = 7;
      code.title="Steane 713 code";
      code.get_713_code();
      break;
    case 2://from file
      code.load(code_prefix);
      code.title=note;
      break;
    default:
      std::cout<<"Hint: choose mode in {0,1,2}; program exit"<<std::endl;
      return 0;
    }

    code.dist();
    code.k = code.n - code.Gx.row_rank() - code.Gz.row_rank();
    std::cout<<code<<std::endl;
    std::cout<<"Finish generating code"<<std::endl;

    code.Gx = common::make_it_full_rank(code.Gx);
    code.Gz = common::make_it_full_rank(code.Gz);
    
    if (debug) std::cout<<"before simulate()"<<std::endl;

    const int num_data=13;
    double p_qubit[num_data], p_block[num_data];
    //    double p = 0.001;
    double p = 0.1;
    std::map<double,double> data_map;
    //    const int e_try = 1000000;//1,000,000 for Steane codes
    for ( int i =0 ; i<num_data; i++){
      p_qubit[i] = p;
      p_block[i] = code.simulate(p, e_try, num_cores, debug); 
      data_map[p_qubit[i]]=p_block[i];
      p /= 1.2;
    }
    
    json::object_t object_value={
      {"data_map",data_map},
      {"note",note},
      {"title",code_prefix},
      {"e_try",e_try},
      {"num_cores",num_cores},
      {"num_data",num_data},
    };
    json j_object_t(object_value);
    std::ofstream filejson(output_json);
    filejson << j_object_t;
    filejson.close();

    std::cout<<"Finish simulation"<<std::endl;
    return 0;

}

