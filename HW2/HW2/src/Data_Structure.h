#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

using namespace std;
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <deque>

struct LibCell
{
    string name;
    long long size;
};

class Tech{
    public:
        Tech();
        Tech(const string& name, int num);
    
        string tech_name; 
        int cell_num; // # of cell in tech
        vector<LibCell> cells; //Std Cell info 

        void build_std_cell(const string&name, long long size);
};
class Die{
    public:
        Die();
        Die(const string& die_name, const string& die_tech, long long condition);
        string name;
        string tech;
        long long area_max; // need area <= util*area(DieMaxArea)
        long long current_area;
};
class Net;
class Cell{
    public:
        Cell();
        Cell(const string& cell_id, const string& std_cell_name);
        string id;
        string cell_type;
        string current_tech;
        vector<Net*> net_list;
        long long size_in_die_A;
        long long size_in_die_B;
        long long gain;
        bool locked = false;
};

class Net{
    public:
        Net();
        Net(string name, vector<Cell*> cells, long long weight);
        vector<Cell*> cell_list;
        string id;
        long long net_weight;
};

class Info{
    public:
        Info();
        vector<Net> nets;
        vector<Cell> cells;
        vector<Die> dies;
        vector<Tech> tech_list;
        vector<Cell*> cells_to_sort; //for sorting 
        unordered_map<string, Cell*> cell_map;
        unordered_map<string, Net*> net_map;
        long long cut_size;

        // Get val. func.
        int get_std_cell_size(string Lib, string LibCell); 
        // Die Initialization method
        void die_initialize();
        bool greedy_die_initialize(); //1 
        bool weighted_die_initialize(); //2: put smaller_size die 
        bool net_balancing_die_initialize(); // 3: made cut_size bigger but converge faster
        bool net_balancing_die_force_init(); // 4: force 3
        // Initialization
        void initialize();
        void gain_initialize();
        // for error check 
        void print_cell_vector(); 
        void print_net_vector();

        // ans.
        void cut_size_compute();
};

class FM_BucketList{
    public:
        FM_BucketList();
        FM_BucketList(Info& info);  

        map<long long, deque<Cell*>, greater<long long>> bucketA;
        map<long long, deque<Cell*>, greater<long long>> bucketB;
        
        /*Records*/
        vector<Cell*> move_squence;
        vector<long long> gain_sequence;
        vector<double> balance_ratio; 
        long long max_gain;
        int max_index;
        long long partial_sum; // accelerate 1
        long long max_gain_unchange; // accelerate 2

        // basic bucketlist op.
        void insert(Cell* cell, string tech);
        Cell* get_max_gain_cell(string tech);
        void remove(Cell* cell);
        void update_cell_gain(Cell* cell, long long new_gain);
        // error check
        void printf_bucket(string tech);
        // FM op
        bool FM(Info& info, int max_neg_partial, int max_neg_gain, int max_abandon_round); // wrap FM algo.
        bool update_gain(Info& info); // T: found valid cell to update
        void update_gain_before_move(Info& info, string tech, Net* net);
        void select_cell_switch_die(Cell* cell);
        void update_gain_after_move(Info& info, string tech, Net* net);
        Cell* select_move_cell(Info& info);
        // FM output
        long long compute_max_gain();
        void rollback(Info& info);
        long long cut_size(Info& info);
        void compute_balance_ratio(Info& info);
};



#endif