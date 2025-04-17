#include <iostream>
#include <fstream>
#include "Data_Loader.h"
#include "Data_Structure.h"
using namespace std;

int main(int argc, char *argv[]){
    
    Info info;
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Init data info
    info.initialize(); // Init T and E
    cout << info.calculate_cost() << endl;
    // SA algo.
    // 5 -> epsilon
    // while(((info.reject/info.MT) <= 0.95) && info.T > 5){

    // }
    

    return 0;
}