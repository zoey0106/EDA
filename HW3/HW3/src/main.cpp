#include <iostream>
#include <fstream>
#include <random>
#include "Data_Loader.h"
#include "Data_Structure.h"
using namespace std;
// do not consider dead-space ratio now
int main(int argc, char *argv[]){
    // Input data
    Info info;
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Init data info
    info.dead_space_ratio = stod(argv[3]);
    // SA algo.
    info.SA_algo(5); //ϵ: threshold

    // compute final wiring length
    write_output(info, argv[2]);

    return 0;
}