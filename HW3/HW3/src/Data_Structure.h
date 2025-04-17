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
        Net() = default;;
        Net(string net_name, vector<string> pin_list);
        string name;
        vector<string> pins; // contain block/pad name
};

class Pad{
    public:
        Pad() = default;;
        Pad(string pad_name, long long x_axis, long long y_axis);
        string name;
        long long x;
        long long y;
};

class HardBlock{
    public:
        HardBlock() = default;
        HardBlock(string block_name, long long w, long long h);
        string name;
        long long width;
        long long height;
        bool rotate;
};

enum class PEType { Operand, H, V };

struct PEItem {
    PEType type;
    HardBlock* hard_block; // if operand

    PEItem(PEType t, HardBlock* block = nullptr): type(t), hard_block(block){};
};

class PolishExpr{
    public:
        vector<PEItem> expr; // solution
        PolishExpr() = default;
        PolishExpr(const vector<PEItem>& items): expr(items) {};

};

class Info{
    public:
        Info() = default;
        // Data
        vector<HardBlock> hard_block_list;
        vector<Net> net_list;
        vector<Pad> pad_list;
        unordered_map<string, Pad*> pad_map;
        unordered_map<string, HardBlock*> hard_block_map;
        unordered_map<string, Net*> net_map;
        // Termination
        int MT; 
        int reject;
        // SA sol. & param.
        double T;
        PolishExpr E;
        PolishExpr best_E;
        long long best_cost;
        // SA algo.
        void SA_algo();
        void M1_move();
        long long calculate_cost();
        long long calculate_wiring_length();
        // SA initialization 
        double initial_temperature(int sample_size, double p = 0.9); // Final optimization
        void initial_PolishExpr(); // Init E
        void initialize();

        // Checkerror
        void print_block(string name);
        void print_pad(string name);
        void print_net_pins(string name);
        void print_E();
};

#endif