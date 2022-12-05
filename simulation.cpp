#include "weilei_lib/weilei_lib.h"  //general include goes to weilei_lib_h
#include "json.hpp"
using json=nlohmann::json;
#include <chrono> //print computation time
#include <ctime>  
//#include <ctime> //to get current time
//using namespace common;


/** Decoding simulation for given CSS code using random window decoder
 *@param debug
 */
int main(int args, char ** argv){
  //generate code or read from file
  //run simulation
  //output result as json
    std::cout<<"============begin simulation========="<<std::endl;
    //read parameter
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
    //json j_object_t(data_map);
    //    char filename_json[256];
    //sprintf(filename_json,"%s-%i.json",filename_prefix.c_str(),j);
    //    std::ofstream filejson(filename_json);
    //    std::ofstream filejson("steane.json");
    std::ofstream filejson(output_json);
    filejson << j_object_t;
    filejson.close();
    //    std::cout<<"saved simulation data to "<<filename_json<<std::endl;

    std::cout<<"finish test_decode"<<std::endl;
    return 0;

}

