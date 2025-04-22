#include "Data_Structure.h" 
#include "Global_param.h"
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
#include <chrono>
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

// SA setting
void SA_Setting::reset(){
    k = 5;
    eps = 1.0;
    r = 0.9;
    T = 1000.0;
}
// Info
// error check

void Info::print_E(){
    for (auto& e: E.expr){
        if (e.type == PEType::Operand) cout << e.hard_block->name << " ";
        if (e.type == PEType::H) cout << "H ";
        if (e.type == PEType::V) cout << "V ";
    }
}

vector<int> Info::initial_operands(PolishExpr &old_E){
    vector<int> operands;
    operands.clear();
    for (int i = 0; i < old_E.expr.size(); i++){
        if (old_E.expr[i].type == PEType::Operand){
            operands.emplace_back(i);
        }
    }
    return operands;
}

vector<pair<int, int>>  Info::initial_chain_operators(PolishExpr old_E){
    vector<pair<int, int>> operator_chains;
    int start = -1;
    for (int i = 0; i < old_E.expr.size(); i++){
        if (old_E.expr[i].type != PEType::Operand){
            if (start == -1) start = i;
        }else{
            if (start != -1){
                operator_chains.emplace_back(start, i-1); // 1 can be chain too
            }
            start = -1;
        }
    }
    if (start != -1){
        operator_chains.emplace_back(start,  old_E.expr.size() - 1);
    }
    return operator_chains;
}

vector<pair<int, int>> Info::initial_op_operands(PolishExpr old_E){
    vector<pair<int, int>> adjacent_op_operands;

    for (int i = 0; i < old_E.expr.size() - 1; ++i) {
        if ((old_E.expr[i].type == PEType::Operand && old_E.expr[i + 1].type != PEType::Operand) ||
            (old_E.expr[i].type != PEType::Operand && old_E.expr[i + 1].type == PEType::Operand)) {
            adjacent_op_operands.emplace_back(i, i + 1);
        }
    }
    return adjacent_op_operands;
}

vector<int> Info::initial_num_operators_in_E(PolishExpr old_E){
    vector<int> num_operators_in_E;
    int operator_count = 0;
    for(auto& e: old_E.expr){
        if (e.type != PEType::Operand){
            operator_count++;
        }
        num_operators_in_E.push_back(operator_count);
    }
    return num_operators_in_E;
}

// SA algo. init

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
        return max(hard_block_list[a].height,hard_block_list[a].width) > max(hard_block_list[b].height,hard_block_list[b].width);
    });

    for (int i = 0; i < sorted.size(); ++i) {
        
        HardBlock &c = hard_block_list[sorted[i]];
        if (c.width > c.height){
            c.rotate = true; // 用比較大的那邊先排
        }
        now_width += c.rotate ? c.height: c.width;
        
        if (now_width > w_h_limit) {
            row_count++;
            if (row_count >= 2) {
                expr.emplace_back(PEType::H);
                horizontal_count++;
                row_count = 1;
            }

            now_width = c.rotate ? c.height: c.width;
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

// void Info::initial_movement_data(){
//     initial_operands(E);
//     initial_chain_operators(E);
//     initial_op_operands(E);
//     initial_num_operators_in_E(E);
// }

void Info::initial_PolishExpr(){
    /*initialize Expression E & data*/
    init_expr();
    best_E = E.expr;
    valid_E = E;
}

// SA algo.

// Area computation for hard blocks
void Info::build_subtree_size_table(PolishExpr &NE) {
    subtree_size_table.clear();
    subtree_size_table.resize(NE.expr.size(), 0);

    function<int(int)> dfs = [&](int idx) -> int {
        if (NE.expr[idx].type == PEType::Operand) {
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

    dfs(NE.expr.size() - 1);  // root
}

int Info::get_subtree_size(int idx) const {
    return subtree_size_table[idx];
}

void Info::build_shape_list_topdown(int &idx, PEType type, PolishExpr& NE) {
    if (idx < 0 || idx >= NE.expr.size()) {
        cout << "[Error] build_shape_list_topdown idx out of bound: " << idx << endl;
        exit(1);
    }
    int iam = idx;
    PEItem& e = NE.expr[idx--];

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
    build_shape_list_topdown(idx, e.type, NE);
    int left = idx;
    build_shape_list_topdown(idx, e.type, NE);

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

void Info::assign_coordinate(int idx, Shape shape, int x, int y, PolishExpr &NE) {
    PEItem& e = NE.expr[idx];

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
                    assign_coordinate(left, l, x, y, NE);
                    assign_coordinate(right, r, x + l.width, y, NE);
                } else {
                    assign_coordinate(left, l, x, y, NE);
                    assign_coordinate(right, r, x, y + l.height, NE);
                }
                return;
            }
        }
    }
}

long long Info::calculate_area_and_axis(PolishExpr &NE) {
    /*  
        Calculate min_area and assign axis to NE
        Input: Polish expresssion NE
        Return min_area
    */
    build_subtree_size_table(NE);
    shape_table.clear();
    shape_table.resize(NE.expr.size());
    int expr_id = NE.expr.size()-1;
    int root_idx = NE.expr.size()-1;
    build_shape_list_topdown(expr_id, NE.expr[expr_id].type, NE);
    const auto& final_shapes = shape_table[root_idx];

    int shape_idx = 0;
    int chosen_idx = -1;
    int sol_idx = -1;
    long long exceed_area = 0;
    long long min_area = LLONG_MAX, sol_min = LLONG_MAX;
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
        } else {
            if (shape.height * shape.width < sol_min) {
                sol_idx = shape_idx;
                sol_min = shape.height * shape.width;
            }
        }
        if (min_area > exceed_area){
            min_area = exceed_area;
            chosen_idx = shape_idx;
        }
        shape_idx++;
    }
    if (sol_idx != -1)
        chosen_idx = sol_idx;
    const Shape* best = &final_shapes[chosen_idx];
    assign_coordinate(root_idx, *best, 0, 0, NE);
    return min_area;
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

// HPWL:can acclerate to not calculate every block
long long Info::calculate_wiring_length(PolishExpr NE){
    /*  NEED to execute func. [calculate_area_and_axis()] first
        Input: Polish expreesion NE
        Return: wiring length of NE    
    */
    unordered_map<string, HardBlock*> block_map;
    for (auto& item : NE.expr) {
        if (item.type == PEType::Operand) {
            block_map[item.hard_block->name] = item.hard_block;
        }
    }

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
            }else if(block_map.count(pin_name)){
                const auto& block = block_map[pin_name];
                
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

long long Info::calculate_cost(PolishExpr NE, bool outline){
    long long min_area = calculate_area_and_axis(NE);
    long long wiring_length = calculate_wiring_length(NE);
    
    if (wiring_length < lowest_length && min_area == 0) {
        valid_flag = true;
        lowest_length = wiring_length;
    }
    else valid_flag = false;

    if (outline) {
        if (min_area == 0){
            return wiring_length;
        }
        else {
            return min_area + 1000000000LL;
        }
    }
    return min_area*128 + wiring_length; 
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

void Info::swap_expr(int i, int j, PolishExpr &old_E){
    PEType temp_type = old_E.expr[i].type;
    old_E.expr[i].type = old_E.expr[j].type;
    old_E.expr[j].type = temp_type;

    HardBlock* temp_block = old_E.expr[i].hard_block;
    old_E.expr[i].hard_block = old_E.expr[j].hard_block;
    old_E.expr[j].hard_block = temp_block;
}

bool Info::is_valid_expr(int i, int j, vector<int> num_operators_in_E){
    /* return true: valid swap
       return false: invalid swap */
    if (num_operators_in_E[j]*2 < i) return true;
    return false;
}

PolishExpr Info::M1_move(PolishExpr old_E){
    /* Randomly swap two operands */
    vector<int> operands = initial_operands(old_E);
    uniform_int_distribution<int> dist(0, operands.size() - 2); 
    int random = dist(gen);
    // swap_expr(operands[random], operands[random+1], old_E);
    swap(old_E.expr[operands[random]], old_E.expr[operands[random+1]]);
    return old_E;
}

PolishExpr Info::M1_random(PolishExpr old_E){
    /* Randomly swap two operands */
    vector<int> operands = initial_operands(old_E);
    uniform_int_distribution<int> dist(0, operands.size() - 1);
    int random_first = dist(gen);
    int random_second;
    do {
        random_second = dist(gen);
    } while (random_first == random_second);
    swap(old_E.expr[operands[random_first]], old_E.expr[operands[random_second]]);
    return old_E;
}

PolishExpr Info::M2_move(PolishExpr old_E){
    vector<pair<int, int>> operator_chains = initial_chain_operators(old_E);

    if (operator_chains.empty()) return old_E;

    int choice = rand() % operator_chains.size();
    auto[start, end] = operator_chains[choice];
    for(int i = start; i <= end; i++){
        if (old_E.expr[i].type == PEType::Operand){
            cout << "WRONG: M2 [Operand in operator_chains]" << endl;      
            exit(1);
        }
        old_E.expr[i].type = invert_type(old_E.expr[i].type);
    }
    return old_E;
}
// how to keep HH or VV outoftheway?
PolishExpr Info::M3_move(PolishExpr old_E){
    vector<pair<int, int>> adjacent_op_operands = initial_op_operands(old_E);
    if (adjacent_op_operands.empty()) return old_E;
    bool flag = true;
    while (flag){
        int choice = rand() % adjacent_op_operands.size();
        auto [i, j] = adjacent_op_operands[choice];
        
        if (E.expr[i].type == PEType::Operand){
            // if VV/HH -> do again
            if (i > 0 && old_E.expr[i-1].type != old_E.expr[j].type){
                flag = false;
            }
            // operand , {V,H} -> check balloting property
            if (!is_valid_expr(i, j, initial_num_operators_in_E(old_E))) {
                flag = true;
            }
        }
        else{
            // if VV/HH -> do again
            if (j < old_E.expr.size()-1 && old_E.expr[i].type != old_E.expr[j+1].type){
                flag = false;
            }
        }
        if (!flag) swap(old_E.expr[i], old_E.expr[j]);
    }
    return old_E;
}

PolishExpr Info::select_move(PolishExpr old_E){
    vector<int> operands = initial_operands(old_E);
    uniform_int_distribution<int> dist(0, 1);
    int move_type = dist(gen);
    switch (move_type){
        case 0: return M1_random(old_E);
        case 1: return M1_move(old_E);
    }
}

double Info::T_secheduling(double T, bool outline){
    return T * setting.r;
}

void Info::SA_algo(bool outline){
    setting.reset();
    if (outline){
        setting.r = 0.9;
        setting.eps = 0.0004;
    }
    else setting.r = 0.997;

    double MT = 0, reject = 0, T = setting.T;
    int epoch = 0, uphill = 0, N = setting.k * hard_block_list.size();
    cout << "N: " << N << endl;
    best_cost = calculate_cost(E, outline);
    long long old_cost = best_cost;
    cout << "Initial best cost: " << best_cost << "\n";
    if (best_cost == 0 && outline){
        cout << "[Success] Outline satisfied. Continuing to minimizing wirelength";
    }

    
    while((reject/(MT == 0 ? 1 : MT)) <= 0.95 && T >= setting.eps){
        MT = 0;
        reject = 0;
        uphill = 0;
        while(uphill <= N && MT <= 2*N){
            PolishExpr NE = select_move(E);
            long long new_cost = calculate_cost(NE, outline);
            long long delta_c = new_cost - old_cost;
            uniform_real_distribution<double> dist(0.0, 1.0);
            MT++;
            if (delta_c <= 0){
                E = NE;
                old_cost = new_cost;

                if (new_cost < best_cost){
                    best_E = NE;
                    best_cost = new_cost;
                    if (valid_flag) valid_E = NE;
                }
            }
            else{
                double random = dist(gen); 
                if (random < exp(-delta_c/T)){
                    uphill++;
                    E = NE;
                    old_cost = new_cost;
                }
                else{
                    reject++;
                }
            } 
        }
        T = T_secheduling(T, outline);
        cout << "Last cost:  " << old_cost << " [ T: " << T << " ]\n";
    }

    E = best_E;
    calculate_area_and_axis(E);
    best_wiring_length = calculate_wiring_length(E);
    cout << "Final wiring length: " << best_wiring_length << "\n";
}
