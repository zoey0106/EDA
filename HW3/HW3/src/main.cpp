#include <iostream>
#include <fstream>
#include "Data_Loader.h"
#include "Data_Structure.h"
using namespace std;

int main(int argc, char *argv[]){

    Info info;
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Initial info
    
    return 0;
}