#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

using namespace std;
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <deque>
#include <set>
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
        long long x;
        long long y;
        bool rotate;
};

enum class PEType { Operand, H, V };

struct Shape{
    long long width;
    long long height;
    bool rotated; 
    // trace info
    HardBlock* hard_block;
    Shape* left_child = nullptr;
    Shape* right_child = nullptr;
    

    bool operator<(const Shape& other)const{
        if (width != other.width) return width < other.width;
        if (height != other.height) return height < other.height;
        return rotated < other.rotated;
    }
};

struct PEItem {
    PEType type;
    HardBlock* hard_block; // if operand
    set<Shape> shape_set;
    PEItem(PEType t, HardBlock* block = nullptr): type(t), hard_block(block){
        if(type == PEType::Operand && block != nullptr){
            shape_set.insert({block->width, block->height, false, block});
            shape_set.insert({block->height,block->width, true, block});
        }
    };
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
        // [Data]
        vector<HardBlock> hard_block_list;
        vector<Net> net_list;
        vector<Pad> pad_list;
        unordered_map<string, Pad*> pad_map;
        unordered_map<string, HardBlock*> hard_block_map;
        unordered_map<string, Net*> net_map;
        // SA movement
        vector<pair<int, int>> adjacent_operands;
        vector<pair<int, int>> operator_chains;
        vector<pair<int, int>> adjacent_op_operands;
        // Termination
        int MT; 
        int reject;
        // SA sol. & param.
        double T;
        PolishExpr E;
        PolishExpr best_E;
        vector<int> num_operators_in_E;
        long long best_cost;

        // [func. for SA algo.]
        void SA_algo();
        void M1_move();
        void M2_move();
        bool M3_move();
        bool is_valid_expr(int i, int j); // for M3
        // Data maintainess
        void update_adjacent_chain_data(int i, int j);
        // Cost
        long long calculate_cost();
        long long calculate_wiring_length();
        void calculate_area_and_axis();
        // Initialization 
        double initial_temperature(int sample_size, double p = 0.9); // Final optimization
        void initial_PolishExpr(); // Init E
        void initialize();
        void initial_adjacent_operands();
        void initial_chain_operators();
        void initial_op_operands();
        void initial_num_operators_in_E();
        // Checkerror
        void print_block(string name);
        void print_pad(string name);
        void print_net_pins(string name);
        void print_E();
};

#endif