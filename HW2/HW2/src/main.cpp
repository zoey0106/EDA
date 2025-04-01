#include <iostream>
#include <vector>
#include <fstream>
#include "Data_Loader.h"
#include "Data_Structure.h" 
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]){
    cout << " ---------------------------------------- " << endl;
    cout << "Test case: " << argv[1] << endl;
    cout << " ---------------------------------------- " << endl;
    auto start = high_resolution_clock::now(); 
    // Data Preprocessing
    Info info; 
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Initial info
    info.die_initialize(); // Initial die partition
    info.initialize(); // Compelete info
    // FM algo.
    int pass = 0;
    Info final_info;
    while(true){
        info.gain_initialize(); // Initial gain
        FM_BucketList Bucket(info); // Init Bucket
        /* records */
        final_info = info;
        /* p2: Max rounds of negative val. partial gain 
           p3: Max rounds of negative val. gain
           p4: Max abandon rounds         */
        if(!Bucket.FM(info, 5, 5, 500)){ // 5 5 400 
            break; // F: G_k <= 0
        }
        pass++;
        auto end = high_resolution_clock::now(); 
        auto duration = duration_cast<milliseconds>(end - start);
        if (duration.count()/1000 > 150 ){
            break;
        }
    }
    // compute final cutsize
    write_output(final_info, argv[2]);
    cout << "FM Final cutsize: " << final_info.cut_size << endl;
    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Execution time: " << duration.count()/1000 << " s" << endl;
    cout << " ---------------------------------------- " << endl;
    return 0;
}