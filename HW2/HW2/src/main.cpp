#include <iostream>
#include <vector>
#include <fstream>
#include "Data_Loader.h"
#include "Data_Structure.h" 
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]){
    // /: add. in linux
    // Data Preprocessing
    Info info; 
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Initial info
    info.die_initialize(); // Initial die partition
    info.initialize(); // Compelete info
    // FM algo.
    info.gain_initialize(); // Initial gain
    FM_BucketList Bucket(info); // Init Bucket
    
    Bucket.printf_bucket("DieA");
    return 0;
}