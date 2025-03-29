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
    // /: add. in linux
    // Data Preprocessing
    Info info; 
    ifstream input = read_file(argv[1]);
    build_data_structure(info, input); // Initial info
    info.die_initialize(); // Initial die partition
    //info.weighted_die_initialize();
    info.initialize(); // Compelete info
    // FM algo.
    int pass = 0;
    Info final_info;
    long long cut_size;
    while(true){
        cout << "[FM Pass " << pass << "]" << endl;
        info.gain_initialize(); // Initial gain
        FM_BucketList Bucket(info); // Init Bucket
        /* records */
        final_info = info;
        if(!Bucket.FM(info, 10, 10, 1000)){ 
            /* 
                p2: 100 -> 1~4 OK   Max rounds of negative val. partial gain 
                p3: 1000 -> 1~4 OK   Max rounds of negative val. gain
            */ 
            break; // F: G_k <= 0
        }
        pass++;
    }
    // compute final cutsize
    write_output(final_info, argv[2]);

    auto end = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Execution time: " << duration.count()/1000 << " s" << endl;
    cout << " ---------------------------------------- " << endl;
    return 0;
}