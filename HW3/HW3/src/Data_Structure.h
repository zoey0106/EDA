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
#include <climits>
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

class SA_Setting{
    public:
        SA_Setting() = default;
        long long k = 5;
        double eps = 1.0;
        double r = 0.9;
        double T = 1500.0;
        void reset();
};

class Info{
    public:
        Info() = default;
        // [Data]
        vector<HardBlock> hard_block_list;
        vector<Net> net_list;
        vector<Pad> pad_list;
        unordered_map<string, Pad*> pad_map;
        unordered_map<string, Net*> net_map;
        // Termination
        SA_Setting setting; 
        // SA sol. & param.
        PolishExpr E;
        PolishExpr best_E;
        long long best_wiring_length;
        long long best_cost;
        // return. Expr
        bool valid_flag = false;
        bool lower_length = false;
        long long lowest_length = LLONG_MAX;
        PolishExpr valid_E;
        // cost
        vector<vector<Shape>> shape_table;
        vector<int> subtree_size_table;
        // constraint
        double dead_space_ratio;
        long long w_h_limit;
        // [func. for SA algo.]
        void SA_algo(bool outline);
        PolishExpr M1_move(PolishExpr old_E);
        PolishExpr M1_random(PolishExpr old_E);
        PolishExpr M2_move(PolishExpr old_E);
        PolishExpr M3_move(PolishExpr old_E);
        PolishExpr select_move(PolishExpr old_E);
        bool is_valid_expr(int i, int j, vector<int> num_operators_in_E); // for M3
        void swap_expr(int i, int j, PolishExpr &old_E);
        // T schedule
        double T_secheduling(double T, bool outline);
        // Cost
        long long calculate_cost(PolishExpr NE, bool outline);
        long long calculate_wiring_length(PolishExpr NE);
        long long calculate_area_and_axis(PolishExpr &NE);
        void assign_coordinate(int idx, Shape shape, int x, int y, PolishExpr &NE);
        void build_shape_list_topdown(int& idx, PEType type, PolishExpr &NE);
        int get_subtree_size(int idx) const;
        void build_subtree_size_table(PolishExpr &NE);
        // constraint
        void get_floorplan_limit();
        bool is_floorplan_within_limit();
        // Initialization 
        // -----initial_movement_data-----//
        vector<int> initial_operands(PolishExpr &old_E);
        vector<pair<int, int>> initial_chain_operators(PolishExpr old_E);
        vector<pair<int, int>> initial_op_operands(PolishExpr old_E);
        vector<int> initial_num_operators_in_E(PolishExpr old_E);
        // Init expr
        void initial_PolishExpr(); // Init E
        void init_expr();
        // Checkerror
        void print_E();
};

#endif