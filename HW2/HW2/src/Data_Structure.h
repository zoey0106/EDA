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

        bool verify_util(); // T : OK
};
class Net;
class Cell{
    public:
        Cell();
        Cell(const string& cell_id);
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

        // Get val. func.
        int get_std_cell_size(string Lib, string LibCell); 
        Cell find_cell(string cell_name);
        Net find_net(string net_name);
        // Initialization
        void die_initialize();
        void initialize();
        void gain_initialize();
        void reset();
        void rebuild_maps();
        Info clone() const;
        // for error check 
        void print_cell_vector(); 
        void print_net_vector();
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
        long long max_gain;
        int max_index;

        // basic bucketlist op.
        void insert(Cell* cell, string tech);
        Cell* get_max_gain_cell(string tech);
        void remove(Cell* cell);
        bool empty(string tech) const;
        void update_gain(Cell* cell, long long gain);
        // error check
        void printf_bucket(string tech);
        // FM op
        void FM(const Info& info); // wrap FM algo.
        Cell* select_move_cell(const Info& info);
};



#endif