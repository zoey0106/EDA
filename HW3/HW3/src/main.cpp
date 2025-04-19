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
    info.SA_algo();
    
    // compute final wiring length
    write_output(info, argv[2]);

    return 0;
}