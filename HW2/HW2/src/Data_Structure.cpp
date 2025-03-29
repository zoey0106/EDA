#include "Data_Structure.h" 
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <climits>
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

// cell
Cell::Cell(){
    id = "NULL";
    cell_type = "NULL";
    current_tech = "NULL";
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
    cut_size = 0;
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

void Info::weighted_die_initialize(){
    /* For Die initialization: accelerate*/
    /* initial cells_to_sort */
    for (auto& cell: cells){
        cell_map[cell.id] = &cell;
        cells_to_sort.push_back(&cell);
    }

    /* Initialize die partition*/
    string Lib_A = tech_list[0].tech_name;
    string Lib_B =  tech_list[1].tech_name;

    for(auto& cell: cells){
        cell.size_in_die_A = get_std_cell_size(Lib_A,cell.cell_type);
        cell.size_in_die_B = get_std_cell_size(Lib_B,cell.cell_type);
    }

    sort(cells_to_sort.begin(), cells_to_sort.end(),[&](const Cell* A, const Cell* B){
        return abs(A->size_in_die_A - A->size_in_die_B) > abs(B->size_in_die_A - B->size_in_die_B);
    });

    long long dieA_area = 0;
    long long dieB_area = 0;
    long long dieA_max_area = dies[0].area_max;
    long long dieB_max_area = dies[1].area_max;

    for (Cell* cell: cells_to_sort){
        bool prefer_A = cell->size_in_die_A < cell->size_in_die_B;
        if (prefer_A && dieA_area + cell->size_in_die_A <= dieA_max_area) {
            cell->current_tech = dies[0].name;
            dieA_area += cell->size_in_die_A;
        } else if (!prefer_A && dieB_area + cell->size_in_die_B <= dieB_max_area) {
            cell->current_tech = dies[1].name;
            dieB_area += cell->size_in_die_B;
        } else if (dieA_area + cell->size_in_die_A <= dieA_max_area) {
            cell->current_tech = dies[0].name;
            dieA_area += cell->size_in_die_A;
        } else if (dieB_area + cell->size_in_die_B <= dieB_max_area) {
            cell->current_tech = dies[1].name;
            dieB_area += cell->size_in_die_B;
        } else {
            cerr << "Weighted Die Init FAIL!" << endl;
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
                    cell.gain+= info_net->net_weight;
                }
                if (T == 0){
                    cell.gain-= info_net->net_weight;
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

void Info::cut_size_compute(){
    long long cut = 0;
    for (const auto& net : nets) {
        string tech = "";
        bool cross = false;
        for (const auto& cell : net.cell_list) {
            if (tech.empty()) tech = cell->current_tech;
            if (cell->current_tech != tech) {
                cross = true;
                break;
            }
        }
        if (cross) cut += net.net_weight;
    }
    cut_size = cut;
}

// FM algo. bucketlist
FM_BucketList::FM_BucketList(){}

FM_BucketList::FM_BucketList(Info& info){
    for (auto& cell: info.cells){   
        insert(&cell , cell.current_tech);
        cell.locked = false;
    }
    max_gain = LLONG_MIN;
    max_index = -1;
    partial_sum = 0;
    max_gain_unchange = 0;
}

// FM basic op.
void FM_BucketList::insert(Cell* cell, string tech){
    if (tech == "DieA"){
        bucketA[cell->gain].push_back(cell);
    }
    else {
        bucketB[cell->gain].push_back(cell);
    }
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

void FM_BucketList::update_cell_gain(Cell* cell, long long new_gain){
    remove(cell);
    cell->gain = new_gain;
    insert(cell, cell->current_tech);
}
// FM error check
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

// FM algo func.
Cell* FM_BucketList::select_move_cell(Info& info){
    /* 
        return : the chosen valid cell & change the current area in both die
        if no valid cell : return nullptr
    */
    Die& dieA = info.dies[0];
    Die& dieB = info.dies[1];

    Cell* select_cell = nullptr;
    long long best_gain = LLONG_MIN;

    // Find max valid gain cell in dieA
    for (const auto& [gain, cell_list]: bucketA){
        for (Cell* cell: cell_list){
            if (cell->locked) continue; ///why do I need this?

            if (dieB.current_area + cell->size_in_die_B <= dieB.area_max){
                if (gain > best_gain){
                    select_cell = cell;
                    best_gain = gain;
                    break;
                }
            }
        }
        if (select_cell) break;
    }
    // Find max valid gain cell in dieB
    int flag = 0;
    for (const auto& [gain, cell_list]: bucketB){
        if (gain < best_gain) {
            break; // means no way better than dieA
        }
        for (Cell* cell: cell_list){
            if (cell->locked) continue;

            if (dieA.current_area + cell->size_in_die_A <= dieA.area_max){
                if (gain > best_gain){
                    select_cell = cell;
                    best_gain = gain;
                    flag = 1;
                    break;
                }
            }
        }
        if (flag == 1) break; // means find better gain
    }
    // change both die area based on chosen cell
    if (select_cell){
        if (flag == 0){ // dieA -> dieB
            dieA.current_area -= select_cell->size_in_die_A;
            dieB.current_area += select_cell->size_in_die_B;
        }
        else if (flag == 1){ // dieB -> dieA
            dieA.current_area += select_cell->size_in_die_A;
            dieB.current_area -= select_cell->size_in_die_B;
        }
    }
    return select_cell;
}

void FM_BucketList::update_gain_before_move(Info& info, string tech, Net* net){
    long long T = 0;
    for (auto& net_cell: net->cell_list){
        if (net_cell->current_tech != tech){
           T++;
           if (T > 1){
                break; //means no update
           }
        }
    }
    if (T == 0){
        for (auto& net_cell: net->cell_list){   
            update_cell_gain(net_cell, net_cell->gain+net->net_weight);
        }
    }
    else if (T == 1){
        for (auto& net_cell: net->cell_list){
            if (net_cell->current_tech != tech){
               update_cell_gain(net_cell, net_cell->gain-net->net_weight);
               break; // only one
            }
        }
    }
}

void FM_BucketList::select_cell_switch_die(Cell* cell){
    /* change cell die */
    remove(cell);
    if (cell->current_tech == "DieA"){
        cell->current_tech = "DieB";
        //insert(cell, "DieB"); // i dont think we need this
    }
    else {
        cell->current_tech = "DieA";
        // maybe don't need insert back?
        //insert(cell, "DieA");
    }
}

void FM_BucketList::update_gain_after_move(Info& info, string tech, Net* net){
    long long F = 0;
    for (auto& net_cell: net->cell_list){
        if (net_cell->current_tech == tech){
           F++;
           if (F > 1){
                break; //means no update
           }
        }
    }
    if (F == 0){
        for (auto& net_cell: net->cell_list){
            update_cell_gain(net_cell, net_cell->gain-net->net_weight);
        }
    }
    else if (F == 1){
        for (auto& net_cell: net->cell_list){
            if (net_cell->current_tech == tech){
               update_cell_gain(net_cell, net_cell->gain+net->net_weight);
               break; // only one
            }
        }
    }
}

bool FM_BucketList::update_gain(Info& info){
    Cell* cell = select_move_cell(info);
    if (cell){
        cell->locked = true;
        gain_sequence.push_back(cell->gain);
        move_squence.push_back(cell);
        partial_sum += cell->gain;  // accelerate 1
        if (partial_sum > max_gain){ // accelerate 3
            max_gain = partial_sum;
            max_gain_unchange = 0;
        }
        else {
            max_gain_unchange++;
        }
        string cell_init_tech = cell->current_tech;
        for (auto& net: cell->net_list){
            update_gain_before_move(info, cell_init_tech, net);
        }
        select_cell_switch_die(cell); 
        for (auto& net: cell->net_list){
            update_gain_after_move(info, cell_init_tech, net);
        }
        return true;
    }
    return false;
}

long long FM_BucketList::compute_max_gain(){
    long long total_gain = 0;
    max_gain = LLONG_MIN;
    max_index = -1;
    
    for (int i = 0; i < gain_sequence.size(); ++i) {
        total_gain += gain_sequence[i];
        if (total_gain > max_gain) {
            max_gain = total_gain;
            max_index = i;
        }
    }
    cout << "gain : " << max_gain << endl;
    return max_gain;
}

void FM_BucketList::rollback(Info& info){
    for (int i = max_index + 1; i < move_squence.size(); ++i) {
        Cell* cell = move_squence[i];
        if (cell->current_tech == info.dies[0].name) {
            cell->current_tech = info.dies[1].name;
            info.dies[0].current_area -= cell->size_in_die_A;
            info.dies[1].current_area += cell->size_in_die_B;
        } else {
            cell->current_tech = info.dies[0].name;
            info.dies[1].current_area -= cell->size_in_die_B;
            info.dies[0].current_area += cell->size_in_die_A;
        }
    }
}

long long FM_BucketList::cut_size(Info& info){
    long long cut = 0;
    for (const auto& net : info.nets) {
        string tech = "";
        bool cross = false;
        for (const auto& cell : net.cell_list) {
            if (tech.empty()) tech = cell->current_tech;
            if (cell->current_tech != tech) {
                cross = true;
                break;
            }
        }
        if (cross) cut += net.net_weight;
    }
    info.cut_size = cut;
    // cout << "FM_cut : " << cut << endl;
    return cut;
}

bool FM_BucketList::FM(Info& info, int max_neg_partial, int max_neg_gain, int max_abandon_round){
    int rounds = 0;
    int downfall = 0;
    while (update_gain(info)){
        // accelerate 1: if partial sum become negative for consecutive "restrict_rounds" -> suspend
        if (partial_sum <= 0){
            rounds++;
        }
        else{
            rounds = 0;
        }        
        if (rounds > max_neg_partial){
            break;
        }
        // accerlerate 2: if partial sum keep becoming lower
        if (gain_sequence.back() < 0){
            downfall++;
        }
        else{
            downfall = 0;
        }
        if (downfall > max_neg_gain){
            break;
        }
        // accerlerate 3: restrict abandon rounds
        if (max_gain_unchange > max_abandon_round){
            break;
        }

    }
    if (compute_max_gain() > 0){
        // cout << "abandon rounds: " << gain_sequence.size() - max_index -1 << endl; 
        rollback(info);
        cut_size(info);
        return true;
    }
    // cout << "Pass over! : G_k <= 0 " << endl;
    return false;
}

