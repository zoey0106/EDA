#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "Data_Loader.h"
#include "Data_Structure.h" 

using namespace std;

vector<string> slice_line(string line){
    istringstream slice(line);
    vector<string> tokens;
    string token;
    while(slice >> token){
        tokens.push_back(token);
    }

    return tokens;
}

Net build_net_info(ifstream& input, vector<string> tokens, Info& info){
    string line;
    string net_name = tokens[1];
    int net_pin_num = stoi(tokens[2]);
    vector<string> pin_list;
    for (int i = 0; i < net_pin_num; i++){
        getline(input, line);
        tokens = slice_line(line);
        pin_list.push_back(tokens[1]);
    }
    Net net(net_name, pin_list);
    return net;
}

Pad build_pad_info(ifstream& input, vector<string> tokens){
    if(tokens[0] != "Pad"){
        cout << "Build Pad error: "<< tokens[0] << endl;
    }
    Pad pad(tokens[1], stoll(tokens[2]), stoll(tokens[3]));
    return pad;
}

HardBlock build_hard_blocks_info(ifstream& input, vector<string> tokens){
    if(tokens[0] != "HardBlock"){
        cout << "Build HardBlock error: "<< tokens[0] << endl;
    }
    HardBlock hard_block(tokens[1], stoll(tokens[2]), stoll(tokens[3]));
    return hard_block;
}

void build_data_structure(Info& info, ifstream& input){
    /*
    Func: Bulid data from data file "input"
    Input: ifstream& data_file [open] 
    Return: NULL
    */
    string line;
    unordered_map<string, function<void(ifstream& input, vector<string>&)>> commandMap; // Build corresponding func. map
    // Build hard blocks
    commandMap["NumHardBlocks"] = [&](ifstream& input, vector<string>& tokens) {
        int hard_blocks_count = stoi(tokens.back());
        for (int i = 0; i < hard_blocks_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            info.hard_block_list.push_back(build_hard_blocks_info(input, tokens));
        }
        for (auto& hard_block: info.hard_block_list){
            info.hard_block_map[hard_block.name] = &hard_block;
        }
    };
    // Build Pads
    commandMap["NumPads"] = [&](ifstream& input, vector<string>& tokens) {
        int pad_count = stoi(tokens.back());
        for (int i = 0; i < pad_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            info.pad_list.push_back(build_pad_info(input, tokens));
        }
        for (auto& pad: info.pad_list){
            info.pad_map[pad.name] = &pad;
        }
    };
    // Build Net
    commandMap["NumNets"] = [&](ifstream& input, vector<string>& tokens) {
        int net_count = stoi(tokens.back());
        for (int i = 0; i < net_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            if(tokens[0] == "Net"){
                info.net_list.push_back(build_net_info(input, tokens, info));
            }
        }
        for (auto& net: info.net_list){
            info.net_map[net.name] = &net;
        }
    };

    // Run MapCommend
    while(getline(input, line)){ 
        if (line.empty()) continue;
        vector<string> tokens = slice_line(line);
        if(commandMap.count(tokens[0])){ // found cmd. to build data
            commandMap[tokens[0]](input, tokens);
        }
    }
}

ifstream read_file(string filename){
    /*
    Func: Read PATH "../testcase/filename"
    Input: string filename 
    Return: Read file
    */
    ifstream input;
    ofstream output;
    string input_file_path = filename;

    input.open(input_file_path);
    // WARNING
    if (input.fail()){
        cout << "Input file opening failed";
        cout << "File path_name: " << input_file_path;
        exit(1);
    }

    return input;
}

void write_output(Info& info, string filename){
    /*
    Func: Write PATH "../output/filename"
    Input: string filename 
    */
    string output_file_path = filename;
    ofstream fout(output_file_path);

    // WARNING
    if (!fout.is_open()){
        cout << "Output file opening failed";
        cout << "File path_name: " << output_file_path;
        exit(1);
    }
    // info.cut_size_compute();
    fout << "Wirelength " << info.best_wiring_length << endl;

    fout << "NumHardBlocks " << info.best_hard_block_list.size() << endl;
    for (int i = 0; i < info.best_hard_block_list.size(); i++){
        fout << info.best_hard_block_list[i].name << " " << info.best_hard_block_list[i].x << " "<< info.best_hard_block_list[i].y << " "<< info.best_hard_block_list[i].rotate << endl;
    }

    fout.close();

}