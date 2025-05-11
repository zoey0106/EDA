#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "Data_Loader.hpp"
#include "DataTypes.hpp" 

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

SymGroup build_sym_group_data(ifstream& input, vector<string> tokens, Info& data){
    string line;
    int sym_block_num = stoi(tokens[2]);
    SymGroup group_list;
    group_list.name = tokens[1];

    for (int i = 0; i < sym_block_num; i++){
        getline(input, line);
        tokens = slice_line(line);
        // symetric pair
        if (tokens[0] == "SymPair"){
            SymPair pair{tokens[1], tokens[2]};
            group_list.pairs.push_back(pair);
        }// symetric self
        else if (tokens[0] == "SymSelf"){
            SymSelf self{tokens[1]};
            group_list.selfs.push_back(self);
        }
        else{
            cout << "ERROR: SymGroup\n";
        }
    }
    return group_list;
}

HardBlock build_hard_blocks_data(ifstream& input, vector<string> tokens){
    if(tokens[0] != "HardBlock"){
        cout << "Build HardBlock error: "<< tokens[0] << endl;
    }
    HardBlock hard_block{tokens[1], stoll(tokens[2]), stoll(tokens[3])};
    return hard_block;
}

void build_data_structure(Info& data, ifstream& input){
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
            data.hard_blocks.push_back(build_hard_blocks_data(input, tokens));
        }
    };
    // Build symmetric group
    commandMap["NumSymGroups"] = [&](ifstream& input, vector<string>& tokens) {
        int symgroup_count = stoi(tokens.back());
        for (int i = 0; i < symgroup_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            if(tokens[0] == "SymGroup"){
                data.sym_groups.push_back(build_sym_group_data(input, tokens, data));
            }
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

void write_output(Info& data, string filename){
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

    // fout << "Area " << bst.getArea() << "\n";
    int64_t maxX = 0, maxY = 0;
    for (const auto& hb : data.hard_blocks) {
        maxX = std::max(maxX, hb.ptr->x_abs + hb.width);
        maxY = std::max(maxY, hb.ptr->y_abs + hb.height);
    }
    
    int64_t totalArea = maxX * maxY;
    fout << "Area " << totalArea << '\n';

    fout << "NumHardBlocks " << data.hard_blocks.size() << "\n";
    for (auto& block: data.hard_blocks){
        fout << block.name << " "
             << block.ptr->x_abs << " "
             << block.ptr->y_abs << " "
             << block.rotate << "\n";
    }

    fout.close();
}