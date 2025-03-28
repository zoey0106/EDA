#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "Data_Loader.h"
#include "Data_Structure.h" 

#define INPUT_DIR "./testcase"
#define OUTPUT_DIR "./output"

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
    int net_cell_num = stoi(tokens[2]);
    vector<Cell*> cell_list;
    long long weight = stoll(tokens.back());
    for (int i = 0; i < net_cell_num; i++){
        getline(input, line);
        tokens = slice_line(line);
        Cell* cell = info.cell_map[tokens[1]];
        cell_list.push_back(cell);
    }
    Net net(net_name, cell_list, weight);
    return net;
}

Cell build_cell_info(ifstream& input, vector<string> tokens){
    if(tokens[0] != "Cell"){
        cout << "Build Cell error: "<< tokens[0] << endl;
    }
    Cell cell(tokens[1], tokens[2]);
    return cell;
}

Die build_die_info(ifstream& input, vector<string> tokens, long long die_area_max){

    string info;
    getline(input, info);
    tokens = slice_line(info);
    Die die(tokens[0], tokens[1], 0.01 * die_area_max * stoll(tokens[2]));

    return die;
}

Tech build_tech_info(ifstream& input, vector<string> tokens){
    string info;
    int tech_num = stoi(tokens.back());
    Tech tech(tokens[1], tech_num);
    long long cell_width = 0;
    long long cell_height = 0;
    for(int i = 0; i < tech_num; i++){
        getline(input, info);
        tokens = slice_line(info);
        //WARNING
        if (tokens[0] != "LibCell"){
            cout << "Build tech wrong!:  " << tokens[0] << endl;
        }
        // Build cells inside tech 
        cell_width = stoll(tokens[2]);
        cell_height = stoll(tokens[3]);
        tech.build_std_cell(tokens[1], cell_width * cell_height);
    }
    return tech;
}

void build_data_structure(Info& info, ifstream& input){
    /*
    Func: Bulid data from data file "input"
    Input: ifstream& data_file [open] 
    Return: NULL
    */
    string line;
    unordered_map<string, function<void(ifstream& input, vector<string>&)>> commandMap; // Build corresponding func. map
    // Build STD tech cell info
    commandMap["NumTechs"] = [&](ifstream& input, vector<string>& tokens) {
        int tech_count = stoi(tokens.back());
        for (int i = 0; i < tech_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            if(tokens[0] == "Tech"){
                info.tech_list.push_back(build_tech_info(input, tokens));
            }
        }
    };
    // Build die constraint
    commandMap["DieSize"] = [&](ifstream& input, vector<string>& tokens) {
        long long die_area_max = stoll(tokens[1])*stoll(tokens[2]);
        for (int i = 0; i < 2; i++){ // Assume die only have 2
            info.dies.push_back(build_die_info(input, tokens, die_area_max));
        }
        
    };
    // Build cell
    commandMap["NumCells"] = [&](ifstream& input, vector<string>& tokens) {
        int cell_count = stoi(tokens.back());
        for (int i = 0; i < cell_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            info.cells.push_back(build_cell_info(input, tokens));
        }
        for (auto& cell: info.cells){
            info.cell_map[cell.id] = &cell;
        }
    };
    // Build Net
    commandMap["NumNets"] = [&](ifstream& input, vector<string>& tokens) {
        int net_count = stoi(tokens.back());
        for (int i = 0; i < net_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            if(tokens[0] == "Net"){
                info.nets.push_back(build_net_info(input, tokens, info));
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
    string input_file_path = string(INPUT_DIR) + "/" + filename;

    input.open(input_file_path);
    // WARNING
    if (input.fail()){
        cout << "Input file opening failed";
        cout << "File path_name: " << input_file_path;
        exit(1);
    }

    return input;
}