#include <iostream>
#include <fstream>
#include "Data_Loader.h"
#include "Data_Structure.h"
using namespace std;
// do not consider dead-space ratio now
int main(int argc, char *argv[]){
    srand(42);
    Info info;
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Init data info
    info.initialize(); // Init T and E
    // cout << "current wiring length: "<< info.calculate_wiring_length() << endl;
    info.SA_algo();
    // SA algo.
    // 5 -> epsilon
    // while(((info.reject/info.MT) <= 0.95) && info.T > 5){

    // }
    

    return 0;
}