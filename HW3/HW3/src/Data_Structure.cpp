#include "Data_Structure.h" 
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <unordered_set>
#include <cfloat>
#include <cmath>
#include <utility>
#include <set>
#include <stack>
#include <functional>
// Net
Net::Net(string net_name, vector<string> pin_list){
    name = net_name;
    pins = pin_list;
}

// Pad
Pad::Pad(string pad_name, long long x_axis, long long y_axis){
    name = pad_name;
    x = x_axis;
    y = y_axis;
}

// HardBlock
HardBlock::HardBlock(string block_name, long long w, long long h){
    name = block_name;
    width = w;
    height = h;
    rotate = false;
    x = -1;
    y = -1;
}

// Info
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

void Info::print_E(){
    for (auto& e: E.expr){
        if (e.type == PEType::Operand) cout << e.hard_block->name << " ";
        if (e.type == PEType::H) cout << "H ";
        if (e.type == PEType::V) cout << "V ";
    }
}

void Info::initial_adjacent_operands(){
    adjacent_operands.clear();
    for (int i = 0; i < E.expr.size(); i++){
        if (E.expr[i].type == PEType::Operand && E.expr[i+1].type == PEType::Operand){
            adjacent_operands.emplace_back(i, i+1);
        }
    }
}

void Info::initial_chain_operators(){
    operator_chains.clear();
    int start = -1;
    for (int i = 0; i < E.expr.size(); i++){
        if (E.expr[i].type != PEType::Operand){
            if (start == -1) start = i;
        }else{
            if (start != -1){
                operator_chains.emplace_back(start, i-1); // 1 can be chain too
            }
            start = -1;
        }
    }
    if (start != -1){
        operator_chains.emplace_back(start,  E.expr.size() - 1);
    }
}

void Info::initial_op_operands(){
    adjacent_op_operands.clear();
    for (int i = 0; i < E.expr.size() - 1; ++i) {
        if ((E.expr[i].type == PEType::Operand && E.expr[i + 1].type != PEType::Operand) ||
            (E.expr[i].type != PEType::Operand && E.expr[i + 1].type == PEType::Operand)) {
            adjacent_op_operands.emplace_back(i, i + 1);
        }
    }
}

void Info::initial_num_operators_in_E(){
    num_operators_in_E.clear();
    int operator_count = 0;
    for(auto& e: E.expr){
        if (e.type != PEType::Operand){
            operator_count++;
        }
        num_operators_in_E.push_back(operator_count);
    }
}

// SA algo. init
void Info::initial_PolishExpr(){
    /*
        initialize Expression E: 12V3V4V...nV
    */

    vector<PEItem> expr;
    bool flag = false;
    for (auto& block: hard_block_list){
        if (flag == false){
            PEItem e(PEType::Operand, &block);
            expr.push_back(e);
            flag = true;
            continue;
        }
        PEItem e(PEType::Operand, &block);
        expr.push_back(e);
        PEItem e2(PEType::V);
        expr.push_back(e2);
    }
    E.expr = expr;
    best_E = expr;
    best_cost = calculate_cost();
    last_cost = best_cost;
    initial_adjacent_operands();
    initial_chain_operators();
    initial_op_operands();
    initial_num_operators_in_E();
    set_best_epression(best_cost);
}

double Info::initial_temperature(int sample_size, double p){
    // final optimization
    return 1000;
}

void Info::initialize(int k){
    initial_PolishExpr(); // Init sol.
    T = initial_temperature(1000, 0.9); // Init T
    N = k*hard_block_list.size();
    MT = 0;
    reject = 0;
}

// SA algo.

// Area computation for hard blocks
void Info::build_subtree_size_table() {
    subtree_size_table.resize(E.expr.size(), 0);

    function<int(int)> dfs = [&](int idx) -> int {
        if (E.expr[idx].type == PEType::Operand) {
            subtree_size_table[idx] = 1;
            return 1;
        }
        int right = idx - 1;
        int right_size = dfs(right);

        int left = idx - 1 - right_size;
        int left_size = dfs(left);

        subtree_size_table[idx] = 1 + left_size + right_size;
        return subtree_size_table[idx];
    };

    dfs(E.expr.size() - 1);  // root
}

int Info::get_subtree_size(int idx) const {
    return subtree_size_table[idx];
}

vector<Shape> Info::prune_dominated_shapes(vector<Shape>& shapes) {
    sort(shapes.begin(), shapes.end(), [](const Shape& a, const Shape& b) {
        return (a.width < b.width) || (a.width == b.width && a.height < b.height);
    });

    vector<Shape> filtered;
    long long min_height = LLONG_MAX;
    for (auto it = shapes.rbegin(); it != shapes.rend(); ++it) {
        if (it->height < min_height) {
            filtered.push_back(*it);
            min_height = it->height;
        }
    }
    reverse(filtered.begin(), filtered.end());
    return filtered;
}

int Info::build_shape_list_topdown(int idx) {
    if (idx < 0 || idx >= E.expr.size()) {
        cout << "[Error] build_shape_list_topdown idx out of bound: " << idx << endl;
        exit(1);
    }
    PEItem& e = E.expr[idx];

    if (e.type == PEType::Operand) {
        HardBlock* block = e.hard_block;
        shape_table[idx] = {
            Shape(block->width, block->height, false),
            Shape(block->height, block->width, true)
        };
        return idx;
    }

    int right = build_shape_list_topdown(idx - 1);
    int left = build_shape_list_topdown(idx - 1 - get_subtree_size(right));

    const auto& L = shape_table[left];
    const auto& R = shape_table[right];
    vector<Shape> result;

    for (const auto& l : L) {
        for (const auto& r : R) {
            if (e.type == PEType::V)
                result.emplace_back(l.width + r.width, max(l.height, r.height), false);
            else
                result.emplace_back(max(l.width, r.width), l.height + r.height, false);
        }
    }

    shape_table[idx] = prune_dominated_shapes(result);
    // shape_table[idx] = result;
    return idx;
}
void Info::assign_coordinate_flat(int idx, Shape shape, int x, int y) {
    PEItem& e = E.expr[idx];

    if (e.type == PEType::Operand) {
        e.hard_block->x = x;
        e.hard_block->y = y;
        e.hard_block->rotate = shape.rotated;
        return;
    }

    int right = idx - 1;
    int left = idx - 1 - get_subtree_size(right);

    const auto& L = shape_table[left];
    const auto& R = shape_table[right];

    for (const auto& l : L) {
        for (const auto& r : R) {
            long long cw, ch;
            if (e.type == PEType::V) {
                cw = l.width + r.width;
                ch = std::max(l.height, r.height);
            } else {
                cw = std::max(l.width, r.width);
                ch = l.height + r.height;
            }
            if (cw == shape.width && ch == shape.height) {
                if (e.type == PEType::V) {
                    assign_coordinate_flat(left, l, x, y);
                    assign_coordinate_flat(right, r, x + l.width, y);
                } else {
                    assign_coordinate_flat(left, l, x, y + r.height);
                    assign_coordinate_flat(right, r, x, y);
                }
                return;
            }
        }
    }
}

void Info::calculate_area_and_axis() {
    build_subtree_size_table();
    shape_table.clear();
    shape_table.resize(E.expr.size());

    int root_idx = build_shape_list_topdown(E.expr.size() - 1);
    const auto& final_shapes = shape_table[root_idx];
    const Shape* best = &(*std::min_element(final_shapes.begin(), final_shapes.end(),
                        [](const Shape& a, const Shape& b) {
                            return a.width * a.height < b.width * b.height;
                        }));

    assign_coordinate_flat(root_idx, *best, 0, 0);
}


void Info::set_best_epression(long long current_cost){
    best_cost = current_cost;
    best_E = E;
    best_hard_block_list = hard_block_list;
}
// HPWL:can acclerate to not calculate every block
long long Info::calculate_wiring_length(){
    int wiring_length = 0;

    for(auto& net: net_list){
        if (net.pins.empty()) continue;

        int min_x = INT_MAX, max_x = INT_MIN;
        int min_y = INT_MAX, max_y = INT_MIN;

        for (const auto& pin_name : net.pins){
            int x = 0, y = 0;

            if (pad_map.count(pin_name)){
                x = pad_map[pin_name]->x;
                y = pad_map[pin_name]->y;
            }else if(hard_block_map.count(pin_name)){
                const auto& block = hard_block_map[pin_name];
                
                int w = block->rotate ? block->height : block->width;
                int h = block->rotate ? block->width : block->height;

                x = block->x + w/2; // floor
                y = block->y + h/2;
            }
            else{
                cout << "ERROR: unknow pin:" << pin_name << endl;
                continue;
            }
            min_x = min(min_x, x);
            max_x = max(max_x, x);
            min_y = min(min_y, y);
            max_y = max(max_y, y);
        }
        wiring_length += (max_x - min_x) + (max_y - min_y);
    }
    return wiring_length;
}

long long Info::calculate_cost(){
    calculate_area_and_axis();
    return calculate_wiring_length();
}

// maintainess for movement
void Info::update_adjacent_chain_data(int i, int j){
    
    // update adjacent_operands
    adjacent_operands.erase(remove_if(adjacent_operands.begin(), adjacent_operands.end(), [&](const pair<int,int>&p){return (p.first >= i-1 && p.first <= j);}),adjacent_operands.end());

    // update adjacent_op_operands
    adjacent_op_operands.erase(remove_if(adjacent_op_operands.begin(), adjacent_op_operands.end(), [&](const pair<int,int>&p){return (p.first >= i-1 && p.first <= j);}),adjacent_op_operands.end());

    for (int k = i-1; k <= j; k++){
        if (k >= 0 && k+1 < E.expr.size()){
            // update adjacent_operands
            if (E.expr[k].type == PEType::Operand && E.expr[k + 1].type == PEType::Operand) {
                adjacent_operands.emplace_back(k, k + 1);
            }
            // update adjacent_op_operands
            bool is_op1 = E.expr[k].type != PEType::Operand;
            bool is_op2 = E.expr[k + 1].type != PEType::Operand;
            if (is_op1 != is_op2) {
                adjacent_op_operands.emplace_back(k, k + 1);
            }
        }
    }

    // update operator_chain
    initial_chain_operators();
    // int start = -1;
    // for (int k = max(0, i - 2); k <= min((int)E.expr.size() - 1, j + 2); k++) {
    //     if (E.expr[k].type != PEType::Operand) {
    //         if (start == -1) start = k;
    //     } else {
    //         if (start != -1) {
    //             if (k - start >= 1) operator_chains.emplace_back(start, k - 1);
    //             start = -1;
    //         }
    //     }
    // }
    // if (start != -1 && E.expr.size() - start >= 1) {
    //     operator_chains.emplace_back(start, E.expr.size() - 1);
    // }
}

// func. for movement
PEType invert_type(PEType type){
    if (type == PEType::Operand) cout << "ERROR: Operand but put in invert type";
    if (type == PEType::H){
        return PEType::V;
    }
    return PEType::H;
}

bool Info::is_valid_expr(int i, int j){
    /* return true: valid swap
       return false: invalid swap */
    if (num_operators_in_E[j]*2 < i) return true;
    return false;
}

bool Info::M1_move(){
    /* Swap two operands */
    if (adjacent_operands.empty()) return false;

    int choice = rand() % adjacent_operands.size();
    auto [i, j] = adjacent_operands[choice];
    swap(E.expr[i], E.expr[j]);
    
    return true;
}

bool Info::M2_move(){
    if (operator_chains.empty()) return false;

    int choice = rand() % operator_chains.size();
    auto[start, end] = operator_chains[choice];
    for(int i = start; i <= end; i++){
        if (E.expr[i].type == PEType::Operand){
            for (auto& [start, end] : operator_chains) {
                for (int i = start; i <= end; i++) {
                    cout << i << ": " << (E.expr[i].type == PEType::Operand ? "Operand" : "Op") << endl;
                }
            }            
        }
        E.expr[i].type = invert_type(E.expr[i].type);
    }
    return true;
}
// how to keep HH or VV outoftheway?
bool Info::M3_move(){
    if (adjacent_op_operands.empty()) return false;

    int choice = rand() % adjacent_op_operands.size();
    auto [i, j] = adjacent_op_operands[choice];
    
    if (E.expr[i].type == PEType::Operand){
        // operand , {V,H} -> check balloting property
        if (!is_valid_expr(i, j)) return false;
        num_operators_in_E[i]++;
        // prevent VV/HH
        if (i > 0 && E.expr[i-1].type == E.expr[j].type){
            // invert itself
            E.expr[j].type = invert_type(E.expr[j].type);
        }
    }
    else{
        num_operators_in_E[i]--;
        // prevent VV/HH
        if (j < E.expr.size()-1 && E.expr[i].type == E.expr[j+1].type){
            // invert itself : my way/ can change it to original
            E.expr[i].type = invert_type(E.expr[i].type);
        }
    }
    swap(E.expr[i], E.expr[j]);
    // will effect adjacent_operands/ operator_chains/ adjacent_op_operands -> change later
    update_adjacent_chain_data(i, j);
    return true;
}
/*
bool Info::selectMove() {
    double r = (double)rand() / RAND_MAX;

    if (r < 0.5) return M1_move();         // 50%
    else if (r < 0.8) return M2_move();    // 30%
    else return M3_move();                // 20%
}
*/
bool Info::select_move(){
    int move_type = rand() % 3;
    switch (move_type){
        case 0: return M1_move();
        case 1: return M2_move();
        case 2: return M3_move();
        default: return false;
    }
}

void Info::SA_algo(int ϵ){
    initialize(5); // Init T/E/M/N : param:k=5 * n-> uphill times
    int r = 0.85;
    cout << "Best wiring length: " << best_cost << endl;
    while((reject/(MT == 0 ? 1 : MT)) <= 0.95 && T >= ϵ){
        MT = 0;
        reject = 0;
        uphill = 0;
        while(uphill <= N && MT <= 2*N){
            shape_table.clear(); subtree_size_table.clear();
            while (!select_move());
            long long current_cost = calculate_cost();
            MT++;
            long long delta_c = current_cost - last_cost;
            double random = (double)rand() / RAND_MAX;
            if (delta_c <= 0 || random < exp(-delta_c/T)){
                if (delta_c > 0){
                    uphill++;
                }
                if (current_cost < best_cost){
                    set_best_epression(current_cost);
                    cout << "Best wiring length: " << best_cost << endl;
                }
            }
            else{
                reject++;
            } 
            last_cost = current_cost;
        }
        T *= r;
    }
}
