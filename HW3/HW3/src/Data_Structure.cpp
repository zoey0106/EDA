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
#include <cassert>
#include <numeric>
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

// void Info::initial_adjacent_operands(){
//     adjacent_operands.clear();
//     int ptr = 0;
//     int last_ptr = 0;
//     while(ptr < E.expr.size()){

//     }
//     for (int i = 0; i < E.expr.size(); i++){
//         if (E.expr[i].type == PEType::Operand && E.expr[i+1].type == PEType::Operand){
//             operands.emplace_back(i, i+1);
//         }
//     }
// }

void Info::initial_operands(){
    operands.clear();
    for (int i = 0; i < E.expr.size(); i++){
        if (E.expr[i].type == PEType::Operand){
            operands.emplace_back(i);
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

// void Info::init_expr(){
//     vector<PEItem> expr;
//     cout << "w_h_limit " << w_h_limit << endl;
//     long long now_width = 0;
//     long long row_count = 0;
//     long long col_count = 0;
//     long long horizontal_count = 0;

//     // sort cell by height (optional)
//     vector<int> sorted(hard_block_list.size());
//     for (int i = 0; i < hard_block_list.size(); ++i) {
//         sorted[i] = i;
//     }

//     sort(sorted.begin(), sorted.end(), [&](int a, int b) {
//         return hard_block_list[a].height > hard_block_list[b].height;
//     });

//     for (int i = 0; i < sorted.size(); ++i) {
        
//         const HardBlock &c = hard_block_list[sorted[i]];
//         now_width += c.width;
        
//         if (now_width > w_h_limit) {
//             row_count++;
//             if (row_count >= 2) {
//                 expr.emplace_back(PEType::H);
//                 horizontal_count++;
//                 row_count = 1;
//             }

//             now_width = c.width;
//             col_count = 0;
//         }

//         expr.emplace_back(PEType::Operand, &hard_block_list[sorted[i]]);
        
//         col_count++;
//         if (col_count >= 2) {
//             expr.emplace_back(PEType::V);
//             col_count = 1;
//             horizontal_count++;
//         }
//     }
//     for (int i = horizontal_count; i < hard_block_list.size() - 1; ++i) {
//         expr.emplace_back(PEType::H);
//     }
//     E.expr = expr;
// }


void Info::init_expr(){
    vector<PEItem> expr;
    cout << "w_h_limit " << w_h_limit << endl;
    long long now_width = 0;
    long long row_count = 0;
    long long col_count = 0;
    long long horizontal_count = 0;

    // sort cell by height (optional)
    vector<int> sorted(hard_block_list.size());
    for (int i = 0; i < hard_block_list.size(); ++i) {
        sorted[i] = i;
    }

    sort(sorted.begin(), sorted.end(), [&](int a, int b) {
        return hard_block_list[a].height > hard_block_list[b].height;
    });

    for (int i = 0; i < sorted.size(); ++i) {
        
        const HardBlock &c = hard_block_list[sorted[i]];
        now_width += c.width;
        
        if (now_width > w_h_limit) {
            row_count++;
            if (row_count >= 2) {
                expr.emplace_back(PEType::H);
                horizontal_count++;
                row_count = 1;
            }

            now_width = c.width;
            col_count = 0;
        }

        expr.emplace_back(PEType::Operand, &hard_block_list[sorted[i]]);
        
        col_count++;
        if (col_count >= 2) {
            expr.emplace_back(PEType::V);
            col_count = 1;
            horizontal_count++;
        }
    }
    for (int i = horizontal_count; i < hard_block_list.size() - 1; ++i) {
        expr.emplace_back(PEType::H);
    }
    E.expr = expr;
}

void Info::initial_movement_data(){
    initial_operands();
    initial_chain_operators();
    initial_op_operands();
    initial_num_operators_in_E();
}

void Info::initial_PolishExpr(){
    /*
        initialize Expression E: 12V3V4V...nV
    */
    init_expr();
    best_E = E.expr;
    best_cost = calculate_cost(1);
    best_wiring_length = current_wiring_length;
    last_cost = best_cost;
    initial_movement_data();
    set_best_epression(best_cost);
}

double Info::initial_temperature(int sample_size, double p){
    // // final optimization
    vector<double> deltas;
    double original_cost = calculate_cost(2);

    for (int i = 0; i < sample_size; ++i) {
        shape_table.clear(); 
        subtree_size_table.clear();
        backup_state();
        while (!select_move()); 
        double new_cost = calculate_cost(2);
        double delta = new_cost - original_cost;

        if (delta > 0) {  
            deltas.push_back(delta);
        }

        rollback_state(); 
    }

    if (deltas.empty()) return 1.0; 

    double avg_uphill = accumulate(deltas.begin(), deltas.end(), 0.0) / deltas.size();
    return -avg_uphill / log(p);
}

void Info::initialize(int k, int phase){
    if (phase == 1){
        // Init width& height limit
        get_floorplan_limit();
        initial_PolishExpr(); // Init sol.
        T = initial_temperature(1000, 0.9); // Init T
        N = k*hard_block_list.size();
        MT = 0;
        reject = 0;
    }
    else{
        T = initial_temperature(1000, 0.9); // Init T
        MT = 0;
        reject = 0;
    }
}

// Rollback
void Info::backup_state(){
    last_state.E = E;
    last_state.hard_block_list = hard_block_list;
    last_state.operands = operands;
    last_state.operator_chains = operator_chains;
    last_state.adjacent_op_operands = adjacent_op_operands;
    last_state.num_operators_in_E = num_operators_in_E;
}

void Info::rollback_state(){
    E = last_state.E;
    hard_block_list = last_state.hard_block_list;
    operands = last_state.operands;
    operator_chains = last_state.operator_chains;
    adjacent_op_operands = last_state.adjacent_op_operands;
    num_operators_in_E = last_state.num_operators_in_E;

    for (auto& block : hard_block_list) {
        hard_block_map[block.name] = &block;
    }
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

void Info::build_shape_list_topdown(int &idx, PEType type) {
    if (idx < 0 || idx >= E.expr.size()) {
        cout << "[Error] build_shape_list_topdown idx out of bound: " << idx << endl;
        exit(1);
    }
    int iam = idx;
    PEItem& e = E.expr[idx--];

    if (e.type == PEType::Operand) {
        HardBlock* block = e.hard_block;
        if (type == PEType::H){
            if (block->width < block->height) {
                shape_table[iam] = {
                    Shape(block->height, block->width, true), 
                    Shape(block->width, block->height, false)
                };
            } else if (block->width > block->height) {
                shape_table[iam] = {
                    Shape(block->width, block->height, false),
                    Shape(block->height, block->width, true)
                };
            } else {
                shape_table[iam] = {
                    Shape(block->width, block->height, false)
                };
            }
        }
        else{
            if (block->width < block->height) {
                shape_table[iam] = {
                    Shape(block->width, block->height, false),
                    Shape(block->height, block->width, true)
                };
            } else if (block->width > block->height) {
                shape_table[iam] = {
                    Shape(block->height, block->width, true),
                    Shape(block->width, block->height, false)
                };
            } else {
                shape_table[iam] = {
                    Shape(block->width, block->height, false)
                };
            }
        }
        return;
    }
    int right = idx;
    build_shape_list_topdown(idx, e.type);
    int left = idx;
    build_shape_list_topdown(idx, e.type);

    const auto& L = shape_table[left];
    const auto& R = shape_table[right];
    vector<Shape> result;

    int l_ptr = 0;
    int r_ptr = 0;

    while(l_ptr < L.size() && r_ptr < R.size()){
        if (e.type == PEType::V){
            result.emplace_back(L[l_ptr].width + R[r_ptr].width, max(L[l_ptr].height, R[r_ptr].height), false);
            if (L[l_ptr].height > R[r_ptr].height) l_ptr++;
            else if (L[l_ptr].height < R[r_ptr].height) r_ptr++;
            else {
                l_ptr++;
                r_ptr++;
            }
        }else{
            result.emplace_back(max(L[l_ptr].width, R[r_ptr].width), L[l_ptr].height + R[r_ptr].height, false);
            if (L[l_ptr].width > R[r_ptr].width) l_ptr++;
            else if (L[l_ptr].width < R[r_ptr].width) r_ptr++;
            else {
                l_ptr++;
                r_ptr++;
            }
        }
    }
    
    if (e.type != type){
        reverse(result.begin(), result.end());
    }
    shape_table[iam] = result;
}

void Info::assign_coordinate(int idx, Shape shape, int x, int y) {
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
                ch = max(l.height, r.height);
            } else {
                cw = max(l.width, r.width);
                ch = l.height + r.height;
            }
            if (cw == shape.width && ch == shape.height) {
                if (e.type == PEType::V) {
                    assign_coordinate(left, l, x, y);
                    assign_coordinate(right, r, x + l.width, y);
                } else {
                    assign_coordinate(left, l, x, y);
                    assign_coordinate(right, r, x, y + l.height);
                }
                return;
            }
        }
    }
}

// void Info::calculate_area_and_axis() {
//     build_subtree_size_table();
//     shape_table.clear();
//     shape_table.resize(E.expr.size());
//     int expr_id = E.expr.size()-1;
//     int root_idx = E.expr.size()-1;
//     build_shape_list_topdown(expr_id, E.expr[expr_id].type);
//     const auto& final_shapes = shape_table[root_idx];

//     const Shape* best = &(*std::min_element(final_shapes.begin(), final_shapes.end(),
//                         [&](const Shape& a, const Shape& b) {
//                             bool valid_a = a.width <= w_h_limit && a.height <= w_h_limit;
//                             bool valid_b = b.width <= w_h_limit && b.height <= w_h_limit;
//                             if (valid_a != valid_b) return valid_a;
//                             return a.width * a.height < b.width * b.height;
//                         }));
//     assign_coordinate(root_idx, *best, 0, 0);
// }

long long Info::calculate_area_and_axis() {
    build_subtree_size_table();
    shape_table.clear();
    shape_table.resize(E.expr.size());
    int expr_id = E.expr.size()-1;
    int root_idx = E.expr.size()-1;
    build_shape_list_topdown(expr_id, E.expr[expr_id].type);
    const auto& final_shapes = shape_table[root_idx];

    int shape_idx = 0;
    int chosen_idx = -1;
    long long exceed_area = 0;
    long long min_area = LLONG_MAX;
    for (const auto& shape: final_shapes){
        exceed_area = 0;
        if (shape.height > w_h_limit && shape.width > w_h_limit){
            exceed_area = shape.height * shape.width - w_h_limit*w_h_limit;
        }
        else if (shape.height > w_h_limit){
            exceed_area = (shape.height - w_h_limit) * w_h_limit;
        }
        else if (shape.width > w_h_limit){
            exceed_area = (shape.width - w_h_limit) * w_h_limit;
        }
        if (min_area > exceed_area){
            min_area = exceed_area;
            chosen_idx = shape_idx;
        }
        shape_idx++;
    }
    const Shape* best = &final_shapes[chosen_idx];
    assign_coordinate(root_idx, *best, 0, 0);
    return min_area;
}

void Info::set_best_epression(long long current_cost){
    best_cost = current_cost;
    best_wiring_length = current_wiring_length;
    best_E = E;
    best_hard_block_list = hard_block_list;
}

bool Info::is_floorplan_within_limit(){
    for (const auto& block : hard_block_list) {
        int w = block.rotate ? block.height : block.width;
        int h = block.rotate ? block.width : block.height;

        if (block.x + w > w_h_limit || block.y + h > w_h_limit) {
            return false;
        }
    }
    return true;
}

long long Info::dead_space_cost(){
    /* return : Sum of "area that are outside of bound"*/
    long long cost = 0;
    for (const auto& block : hard_block_list) {
        int w = block.rotate ? block.height : block.width;
        int h = block.rotate ? block.width : block.height;

        if (block.x + w > w_h_limit) {
            cost += block.x + w - w_h_limit;
        }
        if (block.y + h > w_h_limit) {
            cost += block.y + h - w_h_limit;
        }
    }
    return cost;
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
    current_wiring_length = wiring_length;
    return wiring_length;
}

// long long Info::calculate_cost(int phase){
//     calculate_area_and_axis();
//     if (phase == 1){
//         calculate_wiring_length();
//         return 10* dead_space_cost();
//     }
//     return calculate_wiring_length();
// }

long long Info::calculate_cost(int phase){
    if (phase == 1){
        long long area = calculate_area_and_axis();
        calculate_wiring_length();
        return area;
    }
    return calculate_area_and_axis()*20 + calculate_wiring_length();
}

// maintainess for movement
void Info::update_adjacent_chain_data(int i, int j){
    
    // update operands
    initial_operands();

    // update adjacent_op_operands
    adjacent_op_operands.erase(remove_if(adjacent_op_operands.begin(), adjacent_op_operands.end(), [&](const pair<int,int>&p){return (p.first >= i-1 && p.first <= j);}),adjacent_op_operands.end());

    for (int k = i-1; k <= j; k++){
        if (k >= 0 && k+1 < E.expr.size()){
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
}

// constraint
void Info::get_floorplan_limit(){
    long long total_area = 0;
    for (const auto& block : hard_block_list){
        total_area += block.width * block.height; 
    }
    w_h_limit = floor(sqrt(total_area * (1.0 + dead_space_ratio)));
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

bool Info::M1_move_random(){
    /* Randomly swap two operands */
    if (operands.empty()) return false;

    int first_choice = rand() % operands.size();
    int second_choice = rand() % operands.size();
    swap(E.expr[operands[first_choice]], E.expr[operands[second_choice]]);
    return true;
}

bool Info::M1_move(){
    /* Randomly swap two operands */
    int random = rand() % (operands.size()-1);
    swap(E.expr[operands[random]], E.expr[operands[random+1]]);
    return true;
}

bool Info::M2_move(){
    if (operator_chains.empty()) return false;

    int choice = rand() % operator_chains.size();
    auto[start, end] = operator_chains[choice];
    for(int i = start; i <= end; i++){
        if (E.expr[i].type == PEType::Operand){
            cout << "WRONG: M2 [Operand in operator_chains]" << endl;      
            exit(1);
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
    update_adjacent_chain_data(i, j);
    return true;
}

// bool Info::select_move() {
//     double r = (double)rand() / RAND_MAX;

//     if (r < 0.5) return M1_move();         // 50%
//     else if (r < 0.8) return M2_move();    // 30%
//     else return M3_move();                // 20%
// }

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
    int r = 0.85;
    initialize(5, 1); // Init T/E/M/N : param:k=5 * n-> uphill times
    cout << "Initial valid?" << is_floorplan_within_limit() << endl;
    cout << "Initial best cost: " << best_cost << endl;
    cout << "Initial wiring length: " << best_wiring_length << endl;
    bool flag = is_floorplan_within_limit();
    int j = 0;
    cin >> j;
    /*-------------------Area Minimize Phase--------------------*/
    if (!flag){
        while((reject/(MT == 0 ? 1 : MT)) <= 0.95 && T >= ϵ){
            MT = 0;
            reject = 0;
            uphill = 0;
            while(uphill <= N && MT <= 2*N){
                shape_table.clear(); 
                subtree_size_table.clear();
                backup_state();
                while (!select_move());
                long long current_cost = calculate_cost(1);
                long long delta_c = current_cost - last_cost;
                double random = (double)rand() / RAND_MAX; 
                MT++;
                cout << "[Phase 1] Current cost: " << current_cost << " ";
                if (delta_c <= 0 || random < exp(-delta_c/T)){
                    if (delta_c > 0){
                        uphill++;
                    }
                    if (current_cost < best_cost){
                        set_best_epression(current_cost);
                        cout << "[Phase 1] New best wiring length: " << current_wiring_length << endl;
                    }
                    last_cost = current_cost;
                    cout << " [Accept] \n";
                }
                else{
                    rollback_state();
                    reject++;
                    cout << " [Reject] \n";
                } 
            }
            T *= r;
        }
    }
    /*---------------Wiring length Minimize Phase----------------*/
    if(!flag){
        initialize(5, 2);
    }
    best_cost = calculate_cost(2);
    while((reject/(MT == 0 ? 1 : MT)) <= 0.95 && T >= ϵ){
        MT = 0;
        reject = 0;
        uphill = 0;
        while(uphill <= N && MT <= 2*N){
            shape_table.clear(); 
            subtree_size_table.clear();
            backup_state();
            while (!M1_move());
            long long current_cost = calculate_cost(2);
            long long delta_c = current_cost - last_cost;
            double random = (double)rand() / RAND_MAX;
            cout << "[Phase 2] Current cost: " << current_cost << " ";
            MT++;
            if (delta_c <= 0 || random < exp(-delta_c/T)){
                if (delta_c > 0){
                    uphill++;
                }
                if (current_cost < best_cost){
                    set_best_epression(current_cost);
                    cout << "[Phase 2] New best wiring length: " << current_wiring_length << endl;
                }
                last_cost = current_cost;
                cout << " [Accept] \n";
            }
            else{
                rollback_state();
                reject++;
                cout << " [Reject] \n";
            } 
        }
        T *= r;
    }
    cout << "[SA algo. finish]\n" ;
}
