#include "Data_Structure.h" 
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <unordered_set>
#include <cfloat>
#include <cmath>
// Net
Net::Net(){}
Net::Net(string net_name, vector<string> pin_list){
    name = net_name;
    pins = pin_list;
}

// Pad
Pad::Pad(){}
Pad::Pad(string pad_name, long long x_axis, long long y_axis){
    name = pad_name;
    x = x_axis;
    y = y_axis;
}

// HardBlock
HardBlock::HardBlock(){}
HardBlock::HardBlock(string block_name, long long w, long long h){
    name = block_name;
    width = w;
    height = h;
}

// Info
Info::Info(){}
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
