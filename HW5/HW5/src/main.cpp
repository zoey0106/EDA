#include <iostream>
#include <fstream>
#include "Data_Loader.hpp"
#include "DataTypes.hpp"
using namespace std;

int main(int argc, char *argv[]){
    Info data;
    ifstream input = read_file(argv[1]);
    
    build_data_structure(data, input);
    sort_cell(data); // ascending
    sort_row(data); // ascending
    set_block(data); // block site
    // write_output(data, argv[2]);
    return 0;
}