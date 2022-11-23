#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include "json.hpp"
using json=nlohmann::json;
#include <chrono> //print computation time
#include <ctime>  
//#include <ctime> //to get current time
//using namespace common;



int main(){
  //generate code or read from file
  //run simulation
  //output result as json
    std::cout<<"============begin simulation========="<<std::endl;
    CSSCode code;
    code.n = 7;
    code.title="Quantum hamming 713 code";
    code.get_713_code();
    //  std::cout<<code<<std::endl;
    //  code.full_rank();
    //  code.info();
    //  code.d =
    code.dist();
    code.k = code.n - code.Gx.row_rank() - code.Gz.row_rank();
    //std::cout<<"[7,1,3] Hamming code: dx="<<code.dx<<std::endl;
    std::cout<<code<<std::endl;
    std::cout<<"finish generating Steane code"<<std::endl;

    //  double decode(itpp::GF2mat Gx, itpp::GF2mat Gz, double p)

    code.Gx = common::make_it_full_rank(code.Gx);
    code.Gz = common::make_it_full_rank(code.Gz);
    //  double p_block_2 = code.simulate(p);
    //  double p_block = simulate(code.Gx, code.Gz, p);


      // code.simulate(p);
      //    code.info();
      //    std::cout<<code.Gz<<std::endl;
    

    int num_data=10;
    double p_qubit[num_data], p_block[num_data];
    double p = 0.01;
    std::map<double,double> data_map;
    for ( int i =0 ; i<num_data; i++){
      p_qubit[i] = p;
      p_block[i] = code.simulate(p, 10000); 
      data_map[p_qubit[i]]=p_block[i];
      p += 0.01;
    }
    
    json::object_t object_value={
      {"data_map",data_map},
      {"note","something"},
    };
    json j_object_t(object_value);
    //json j_object_t(data_map);
    //    char filename_json[256];
    //sprintf(filename_json,"%s-%i.json",filename_prefix.c_str(),j);
    //    std::ofstream filejson(filename_json);
    std::ofstream filejson("tmp.json");
    filejson << j_object_t;
    filejson.close();
    //    std::cout<<"saved simulation data to "<<filename_json<<std::endl;

    /*
    for ( double pp = 0.01; pp < 0.11; pp += 0.01 ){
      code.simulate(pp);
      break;
    }
    */
    std::cout<<"finish test_decode"<<std::endl;
    return 0;

}

