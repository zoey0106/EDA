#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

using namespace std;
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <deque>

class Net{
    public:
        Net();
        Net(string net_name, vector<string> pin_list);
        string name;
        vector<string> pins; // contain block/pad name
};

class Pad{
    public:
        Pad();
        Pad(string pad_name, long long x_axis, long long y_axis);
        string name;
        long long x;
        long long y;
};

class HardBlock{
    public:
        HardBlock();
        HardBlock(string block_name, long long w, long long h);
        string name;
        long long width;
        long long height;
};

class Info{
    public:
        Info();
        vector<HardBlock> hard_block;
        vector<Net> net;
        vector<Pad> pad;
        unordered_map<string, Pad*> pad_map;
        unordered_map<string, HardBlock*> hard_block_map;
        unordered_map<string, Net*> net_map;
        // checkerror
        void print_block(string name);
        void print_pad(string name);
        void print_net_pins(string name);
};


#endif