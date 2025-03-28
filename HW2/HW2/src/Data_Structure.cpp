#include "Data_Structure.h" 
#include <iostream>
#include <unordered_map>
#include <algorithm>
// tech
Tech::Tech(){
    tech_name = "DefaultName";
    cell_num = 0;
}

Tech::Tech(const string& name, int num){
    tech_name = name;
    cell_num = num;
}

void Tech::build_std_cell(const string&name, long long size){
    cells.emplace_back(LibCell{name,size});
}

// die
Die::Die(){
    name = "DefaultName";
    tech = "NULL";
    area_max = 0;
    current_area = 0;
}

Die::Die(const string& die_name, const string& die_tech, long long condition){

    name = die_name;
    tech = die_tech;
    area_max = condition;
    current_area = 0;
}

bool Die::verify_util(){
    if(current_area > area_max){
        return false;
    }
    return true;
}

// cell
Cell::Cell(){
    id = "NULL";
    cell_type = "NULL";
    current_tech = "NULL";
    gain = 0;
}

Cell::Cell(const string& cell_id){
    id = cell_id;
    gain = 0;
}

Cell::Cell(const string& cell_id, const string& std_cell_name){
    id = cell_id;
    cell_type = std_cell_name;
    gain = 0;
}

// net 
Net::Net(){
    
}

Net::Net(string name, vector<Cell*> cells, long long weight){
    id = name;
    cell_list = cells;
    net_weight = weight;
}


// info 
Info::Info(){
    
}
// info get val.
int Info::get_std_cell_size(string Lib, string LibCell){
    for (const auto& tech: tech_list){
        if(tech.tech_name == Lib){
            for (const auto& cell: tech.cells){
                if(cell.name == LibCell){
                    return cell.size;
                }
            }
        }
    }
    cerr << "Can't find cell: " << Lib << "/" << LibCell << endl;
    return -1;
}

Cell Info::find_cell(string cell_name){
    for (const auto& cell: cells){
        if (cell.id == cell_name){
            return cell;
        }
    }
    return Cell();
}

Net Info::find_net(string net_name){
    for (const auto& net: nets){
        if (net.id == net_name){
            return net;
        }
    }
    return Net();
}
// info init.
void Info::initialize(){
    /*
    connect nets and cells and its maps
     */

    // initialize hash cell map
    for (auto& cell: cells){
        cell_map[cell.id] = &cell;
    }
    
    // initialize nets
    for (auto& net: nets){
        net_map[net.id] = &net;
        for (auto& cell: net.cell_list){
            if (cell_map.count(cell->id)) {
                Cell* info_cell = cell_map[cell->id];
                info_cell->net_list.push_back(&net);
            } else {
                cerr << "Can't find cell: " << cell->id << endl;
            }
        }
    }
}

void Info::die_initialize(){
    /* initial cells_to_sort */
    for (auto& cell: cells){
        cell_map[cell.id] = &cell;
        cells_to_sort.push_back(&cell);
    }
    /* Initialize die partition*/
    string Lib_A = tech_list[0].tech_name;
    string Lib_B =  tech_list[1].tech_name;
    long long prefer_A;

    for(auto& cell: cells){
        cell.size_in_die_A = get_std_cell_size(Lib_A,cell.cell_type);
        cell.size_in_die_B = get_std_cell_size(Lib_B,cell.cell_type);
    }

    sort(cells_to_sort.begin(), cells_to_sort.end(),[&](const Cell* A, const Cell* B){
        return max(A->size_in_die_A ,A->size_in_die_B) > max(B->size_in_die_A ,B->size_in_die_B);
    });

    long long dieA_area = 0;
    long long dieB_area = 0;
    long long dieA_max_area = dies[0].area_max;
    long long dieB_max_area = dies[1].area_max;

    for (Cell* cell: cells_to_sort){
        
        if ((dieA_area + cell->size_in_die_A ) <= dieA_max_area){
            dieA_area += cell->size_in_die_A;
            cell->current_tech = dies[0].name;
        }
        else if((dieB_area + cell->size_in_die_B ) <= dieB_max_area){
            dieB_area += cell->size_in_die_B;
            cell->current_tech = dies[1].name;
        }
        else{
            cerr << "Initialize die FAIL!" << endl;
            exit(-1);
        }
    }
    dies[0].current_area = dieA_area;
    dies[1].current_area = dieB_area;

    cout << "Initialize die area:" <<endl; 
    cout << "dieA max area: " << dieA_max_area << "/ dieA area: " << dieA_area <<endl; 
    cout << "dieB max area: " << dieB_max_area << "/ dieB area: " << dieB_area << endl; 

}

void Info::gain_initialize(){
    
    for (auto& cell: cells){
        cell.gain = 0; 
        for (auto& net: cell.net_list){  
            if (net_map.count(net->id)){
                Net* info_net = net_map[net->id];
                long long F = 0;
                long long T = 0;  
                for (auto& cell_in_info_net: info_net->cell_list){
                    if (cell_in_info_net->current_tech == cell.current_tech){
                        F++;
                    }
                    else { 
                        T++;
                    }
                    // for speed up
                    if (F > 1 && T > 0) break;
                }
                if (F == 1){
                    cell.gain++;
                }
                if (T == 0){
                    cell.gain--;
                }
            }
        }
    }
}
// info error check
void Info::print_cell_vector(){
    for (auto& cell: cells){
        cout << "cell name: " << cell.id << endl;
        cout << "cell type: " << cell.cell_type << endl;
        cout << "cell current tech: " << cell.current_tech << endl;cout << "cell current gain: " << cell.gain << endl;
        cout << "connected nets: " ;
        for (auto& net: cell.net_list){
            cout << net->id << " " ;
        }
        cout << endl;
    }
}

void Info::print_net_vector(){
    for (auto& net: nets){
        cout << "net name: " << net.id << endl;
        cout << "net weight: " << net.net_weight << endl;
        cout << "connected cells: " ;
        for (auto& cell: net.cell_list){
            cout << cell->id << " " ;
        }
        cout << endl;
    }
}
//info FM algo.
void Info::reset(){
    for(auto& cell: cells){
        cell.gain = 0;
        cell.locked = false;
    }
    initialize();
}

// FM algo. bucketlist
FM_BucketList::FM_BucketList(){}

FM_BucketList::FM_BucketList(Info& info){
    for (auto& cell: info.cells){   
        insert(&cell , cell.current_tech);
    }
}

void FM_BucketList::insert(Cell* cell, string tech){
    if (tech == "DieA"){
        bucketA[cell->gain].push_back(cell);
    }
    else {
        bucketB[cell->gain].push_back(cell);
    }
}

bool FM_BucketList::empty(string tech) const {
    if (tech == "DieA"){
        return bucketA.empty();
    }
    return bucketB.empty();
}

Cell* FM_BucketList::get_max_gain_cell(string tech){
    if (tech == "DieA"){
        if (bucketA.empty()) return nullptr;
        auto& top_list = bucketA.begin()->second;
        Cell* top_cell = top_list.front();
        top_list.pop_front();
        if (top_list.empty()){
            bucketA.erase(bucketA.begin());
        }
        return top_cell;
    }
    else{
        if (bucketB.empty()) return nullptr;
        auto& top_list = bucketB.begin()->second;
        Cell* top_cell = top_list.front();
        top_list.pop_front();
        if (top_list.empty()){
            bucketB.erase(bucketB.begin());
        }
        return top_cell;
    }
    return nullptr;
}

void FM_BucketList::remove(Cell* cell){
    // find bucket
    map<long long, deque<Cell*>, greater<long long>>* def_bucket;
    if (cell->current_tech == "DieA"){
        def_bucket = &bucketA;
    } 
    else{
        def_bucket = &bucketB;
    }
    auto& bucket = *def_bucket;
    // remove
    auto gain_bucket = bucket.find(cell->gain);
    if (gain_bucket != bucket.end()){
        auto& cell_list = gain_bucket->second;
        cell_list.erase(std::remove(cell_list.begin(), cell_list.end(), cell),
        cell_list.end());

        if (cell_list.empty()){
            bucket.erase(gain_bucket);
        }
    }

}

void FM_BucketList::printf_bucket(string tech){
    map<long long, deque<Cell*>, greater<long long>>* def_bucket;
    if (tech == "DieA"){
        def_bucket = &bucketA;
    } 
    else{
        def_bucket = &bucketB;
    }
    auto& bucket = *def_bucket;
    cout << "-----------Bucket (" << tech << ") -----------" << endl;

    for (const auto& [gain, cell_list] : bucket) {
        cout << "  Gain = " << gain << " -> ";
        for (const auto& cell : cell_list) {
            cout << cell->id << " ";
        }
        cout << endl;
    }

    cout << "-----------------------------" << endl;
}

void FM_BucketList::update_gain(Cell* cell, long long gain){
    remove(cell);
    cell ->gain = gain;
    insert(cell, cell->current_tech);
}