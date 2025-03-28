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
    info.gain_initialize(); // Initial gain
    // FM algo.
    while(1){
        Info FM_info = info.clone();
        FM_BucketList Bucket(FM_info); // Init Bucket
        if(!Bucket.FM(FM_info, info)){
            break; // F: G_k <= 0
        } // based on FM_info to mimic transaction and show the results in info
    }
    return 0;
}