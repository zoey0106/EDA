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
    // until G_K <= 0

    while(1){
        Info FM_info = info.clone();
        FM_BucketList Bucket(FM_info); // Init Bucket
        Bucket.FM(FM_info); // based on FM info to mimic transaction


        break;
    }

    return 0;
}