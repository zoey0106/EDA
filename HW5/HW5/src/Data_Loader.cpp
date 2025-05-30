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
typedef int64_t ll;

vector<string> slice_line(string line){
    istringstream slice(line);
    vector<string> tokens;
    string token;
    while(slice >> token){
        tokens.push_back(token);
    }
    return tokens;
}

Row build_row(ifstream& input, vector<string> tokens, Info& data){
    if(tokens[0] != "Row"){
        cout << "Build Row error: "<< tokens[0] << endl;
    }
    ll site_width = stoll(tokens[2]);
    ll x = stoll(tokens[4]);
    ll y = stoll(tokens[5]);
    ll site_number = stoll(tokens[6]);
    vector<Site> sites;
    for (ll i = 0; i < site_number; i++){
        Site site{x, y};
        sites.push_back(site);
        x += site_width;
    }
    ll row_width = site_number * site_width;
    Row row{tokens[1], stoll(tokens[2]), row_width, stoll(tokens[3]), stoll(tokens[4]), stoll(tokens[5]), sites};
    return row;
}

Blockage build_blockage(ifstream& input, vector<string> tokens, Info& data){
    if(tokens[0] != "Blockage"){
        cout << "Build Blockage error: "<< tokens[0] << endl;
    }
    Blockage blockage{tokens[1], stoll(tokens[2]), stoll(tokens[3]), stoll(tokens[4]), stoll(tokens[5])};
    return blockage;
}

Cell build_cell(ifstream& input, vector<string> tokens){
    if(tokens[0] != "Cell"){
        cout << "Build Cell error: "<< tokens[0] << endl;
    }
    Cell cell{tokens[1], stoll(tokens[2]), stoll(tokens[3]), stod(tokens[4]), stod(tokens[5]), 0, 0};
    return cell;
}

void build_data_structure(Info& data, ifstream& input){
    /*
    Func: Bulid data from data file "input"
    Input: ifstream& data_file [open] 
    Return: NULL
    */
    string line;
    unordered_map<string, function<void(ifstream& input, vector<string>&)>> commandMap; // Build corresponding func. map
    commandMap["NumCells"] = [&](ifstream& input, vector<string>& tokens) {
        int cells_count = stoi(tokens.back());
        for (int i = 0; i < cells_count; i++){
            getline(input, line);
            tokens = slice_line(line);
            data.cells.push_back(build_cell(input, tokens));
        }
    };
    commandMap["NumBlockages"] = [&](ifstream& input, vector<string>& tokens) {
        int blockage_count = stoi(tokens.back());
        for (int i = 0; i < blockage_count; i++){
            getline(input, line);
            tokens = slice_line(line);           
            data.blockages.push_back(build_blockage(input, tokens, data));
        }
    };
    commandMap["NumRows"] = [&](ifstream& input, vector<string>& tokens) {
        int row_count = stoi(tokens.back());
        for (int i = 0; i < row_count; i++){
            getline(input, line);
            tokens = slice_line(line);           
            data.rows.push_back(build_row(input, tokens, data));
        }
    };
    commandMap["MaxDisplacementConstraint"] = [&](ifstream& input, vector<string>& tokens) {
        data.max_displaced = stoll(tokens[1]);
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

    // fout << "Area " << data.best_area << '\n';

    // fout << "NumHardBlocks " << data.output.size() << "\n";
    // for (auto& out: data.output){
    //     fout << out.name << " "
    //          << out.x << " "
    //          << out.y << " "
    //          << out.rotate << "\n";
    // }

    fout.close();
}