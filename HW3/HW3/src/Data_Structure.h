#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

using namespace std;
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <deque>
#include <set>
#include <memory>
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
    bool rotated = false; 

    Shape() = default;
    Shape(long long w, long long h, bool r = false)
        : width(w), height(h), rotated(r) {}

    bool operator<(const Shape& other) const {
        return tie(width, height, rotated) < tie(other.width, other.height, other.rotated);
    }
};

struct PEItem {
    PEType type;
    HardBlock* hard_block; // if operand
    set<Shape> shape_set;
    PEItem(PEType t, HardBlock* block = nullptr): type(t), hard_block(block){
        if(type == PEType::Operand && block != nullptr){
            shape_set.insert({block->width, block->height, false});
            shape_set.insert({block->height,block->width, true});
        }
    };
};

class PolishExpr{
    public:
        vector<PEItem> expr; // solution
        PolishExpr() = default;
        PolishExpr(const vector<PEItem>& items): expr(items) {};
};

struct PolishState{
    PolishExpr E;
    vector<HardBlock> hard_block_list;

    vector<int> operands;
    vector<std::pair<int, int>> operator_chains;
    vector<std::pair<int, int>> adjacent_op_operands;
    vector<int> num_operators_in_E;
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
        vector<int> operands;
        vector<pair<int, int>> operator_chains;
        vector<pair<int, int>> adjacent_op_operands;
        // Termination
        double MT; 
        double reject;
        int N;
        int uphill;
        // Rollback
        PolishState last_state;
        // SA sol. & param.
        double T;
        PolishExpr E;
        long long current_wiring_length;
        long long last_cost;
        vector<int> num_operators_in_E;
        PolishExpr best_E;
        long long best_wiring_length;
        long long best_cost;
        vector<HardBlock> best_hard_block_list; 
        // cost
        vector<vector<Shape>> shape_table; //record shape table
        vector<int> subtree_size_table; //record subtree  
        // constraint
        double dead_space_ratio;
        long long w_h_limit;
        // [func. for SA algo.]
        void SA_algo(int ϵ);
        bool M1_move();
        bool M2_move();
        bool M3_move();
        bool select_move();
        bool is_valid_expr(int i, int j); // for M3
        // Rollback
        void backup_state();
        void rollback_state();
        // Data maintainess
        void update_adjacent_chain_data(int i, int j);
        // Cost
        long long calculate_cost(int phase);
        long long calculate_wiring_length();
        void calculate_area_and_axis();
        void set_best_epression(long long current_cost);
        void assign_coordinate(int idx, Shape shape, int x, int y);
        void build_shape_list_topdown(int& idx, PEType type);
        int get_subtree_size(int idx) const;
        void build_subtree_size_table();
        // constraint
        void get_floorplan_limit();
        long long dead_space_cost();
        bool is_floorplan_within_limit();
        // Initialization 
        double initial_temperature(int sample_size, double p = 0.9); // Final optimization
        void initialize(int k, int phase);
        void initial_movement_data();
        // -----initial_movement_data-----//
        void initial_operands();
        void initial_chain_operators();
        void initial_op_operands();
        void initial_num_operators_in_E();
        // -----initial_movement_data-----//
        // Init expr
        void initial_PolishExpr(); // Init E
        void init_expr();
        // Checkerror
        void print_block(string name);
        void print_pad(string name);
        void print_net_pins(string name);
        void print_E();
};

#endif