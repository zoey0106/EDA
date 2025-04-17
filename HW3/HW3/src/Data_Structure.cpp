#include "Data_Structure.h" 
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <unordered_set>
#include <cfloat>
#include <cmath>
// Net
Net::Net(string net_name, vector<string> pin_list){
    name = net_name;
    pins = pin_list;
}

// Pad
Pad::Pad(string pad_name, long long x_axis, long long y_axis){
    name = pad_name;
    x = x_axis;
    y = y_axis;
}

// HardBlock
HardBlock::HardBlock(string block_name, long long w, long long h){
    name = block_name;
    width = w;
    height = h;
    rotate = false;
}

// Info
// error check
void Info::print_block(string name){
    if (hard_block_map.count(name)){
        cout << "name: " << hard_block_map[name]->name << endl;
        cout  << "width: "<< hard_block_map[name]->width << endl;
        cout  << "height: "<< hard_block_map[name]->height << endl;
    }
    else{
        cout << "ERROR: No " << name << " exits in the block map" << endl;
    }
}

void Info::print_pad(string name){
    if (pad_map.count(name)){
        cout << "name: " << pad_map[name]->name << endl;
        cout  << "width: "<< pad_map[name]->x << endl;
        cout  << "height: "<< pad_map[name]->y << endl;
    }
    else{
        cout << "ERROR: No " << name << " exits in the pad map" << endl;
    }
}

void Info::print_net_pins(string name){
    if (net_map.count(name)){
        cout << "name: " << net_map[name]->name << endl;
        for (auto& pin: net_map[name]->pins){
            cout << "pin " << pin << endl;
        }
    }
    else{
        cout << "ERROR: No " << name << " exits in the net map" << endl;
    }
}

void Info::print_E(){
    for (auto& e: E.expr){
        if (e.type == PEType::Operand) cout << e.hard_block->name << " ";
        if (e.type == PEType::H) cout << "H ";
        if (e.type == PEType::V) cout << "V ";
    }
}

// SA algo. init
void Info::initial_PolishExpr(){
    /*
        initialize Expression E: 12V3V4V...nV
    */

    vector<PEItem> expr;
    bool flag = false;
    for (auto& block: hard_block_list){
        if (flag == false){
            PEItem e(PEType::Operand, &block);
            expr.push_back(e);
            flag = true;
            continue;
        }
        PEItem e(PEType::Operand, &block);
        expr.push_back(e);
        PEItem e2(PEType::V);
        expr.push_back(e2);
    }
    E.expr = expr;
    best_E = expr;
}

double Info::initial_temperature(int sample_size, double p){
    // final optimization
    return 1000;
}

void Info::initialize(){
    initial_PolishExpr(); // Init sol.
    T = initial_temperature(1000, 0.9); // Init T
    MT = 0;
    reject = 0;
}

// SA algo.
// HPWL
long long Info::calculate_wiring_length(){
    int wiring_length = 0;

    for(auto& net: net_list){
        if (net.pins.empty()) continue;

        int min_x = INT_MAX, max_x = INT_MIN;
        int min_y = INT_MAX, max_y = INT_MIN;

        for (const auto& pin_name : net.pins){
            int x = 0, y = 0;

            if (pad_map.count(pin_name)){
                x = pad_map[pin_name]->x;
                y = pad_map[pin_name]->y;
            }else if(hard_block_map.count(pin_name)){
                const auto& block = hard_block_map[pin_name];
                // add Area computation -> so x, y in block can be decide
            }
        }
    }


}

long long Info::calculate_cost(){
    

}

void Info::M1_move(){
    
}

void Info::SA_algo(){
    print_E();
    M1_move();
    print_E();
}
