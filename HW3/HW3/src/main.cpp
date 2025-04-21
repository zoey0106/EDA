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
    srand(97);
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Init data info
    info.dead_space_ratio = stod(argv[3]);
    // SA algo.
    info.get_floorplan_limit();
    info.initial_PolishExpr(); // Init sol.
    
    cout << "[Valid floorplan finding....] \n";
    info.SA_algo(true); // Goal: find valid ANS.
    cout << "[Minimizing wiring length....] \n";
    info.SA_algo(false); // Goal: minimize wiring length
    cout << "[Valid floorplan finding....] \n";
    info.SA_algo(true); // Goal: find valid ANS.

    // compute final wiring length
    write_output(info, argv[2]);

    return 0;
}