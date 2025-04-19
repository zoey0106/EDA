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
    initial_adjacent_operands();
    initial_chain_operators();
    initial_op_operands();
    initial_num_operators_in_E();
}

double Info::initial_temperature(int sample_size, double p){
    // final optimization
    return 1000;
}

void Info::initialize(){
    initial_PolishExpr(); // Init sol.
    T = initial_temperature(1000, 0.9); // Init T
    MT = 0;
    reject = 0;
}

// SA algo.

// Area computation for hard blocks
void assign_coordinate(Shape* shape, int x, int y){
    if (!shape) return;

    if (shape->hard_block != nullptr){
        shape->hard_block->x = x;
        shape->hard_block->y = y;
        shape->hard_block->rotate = shape->rotated;
        return;
    }

    if (shape->left_child && shape->right_child){
        Shape* L = shape->left_child;
        Shape* R = shape->right_child;

        if (shape->width == L->width + R->width && shape->height == max(L->height, R->height)){
            assign_coordinate(L, x, y);
            assign_coordinate(R, x + L->width, y);
        }else if (shape->height == L->height + R->height && shape->width == max(L->width, R->width)){
            assign_coordinate(R, x, y);
            assign_coordinate(L, x, y + R->height); 
        }else{
            cout << "ERROR: wrong assignment of x,y to block!" << endl;
        }
    }
}

void Info::set_best_epression(long long current_cost){
    best_cost = current_cost;
    best_E = E;
    best_hard_block_list = hard_block_list;
}

void Info::calculate_area_and_axis(){
    stack<set<Shape>> area;
    bool flag = true;

    for(auto& e: E.expr){
        if(e.type == PEType::Operand){
            area.push(e.shape_set);
            continue;
        }
        set<Shape> right_child = area.top(); area.pop();
        set<Shape> left_child = area.top(); area.pop();
        set<Shape> shape_set;

        for(auto& r_shape: right_child){
            for(auto& l_shape: left_child){
                Shape s;
                if(e.type == PEType::V){
                    s.width = r_shape.width+l_shape.width;
                    s.height = max(r_shape.height, l_shape.height);
                }
                else if (e.type == PEType::H){
                    s.width = max(r_shape.width, l_shape.width);
                    s.height = r_shape.height + l_shape.height;
                }
                s.hard_block = e.hard_block;
                s.left_child = new Shape(l_shape);
                s.right_child = new Shape(r_shape);
                shape_set.insert(s);
            }
        }
        e.shape_set = shape_set;
        area.push(shape_set);
    }

    if (area.size() != 1){
        cout << "ERROR:  wrong area computation, now have " << area.size() << " in the stack which should be 1" << endl;
        return;
    }

    set<Shape> final_shape = area.top(); area.pop();
    Shape best_shape = *min_element(final_shape.begin(),final_shape.end(), [](const Shape& a, const Shape& b){ return (a.width * a.height) < (b.width * b.height);});
    // Set hard blocks' x-coordinate & y-coordinate
    assign_coordinate(&best_shape, 0, 0);
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
    int start = -1;
    for (int k = max(0, i - 2); k <= min((int)E.expr.size() - 1, j + 2); k++) {
        if (E.expr[k].type != PEType::Operand) {
            if (start == -1) start = k;
        } else {
            if (start != -1) {
                if (k - start >= 1) operator_chains.emplace_back(start, k - 1);
                start = -1;
            }
        }
    }
    if (start != -1 && E.expr.size() - start >= 1) {
        operator_chains.emplace_back(start, E.expr.size() - 1);
    }
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

void Info::M1_move(){
    /* Swap two operands */
    if (adjacent_operands.empty()) return;

    int choice = rand() % adjacent_operands.size();
    auto [i, j] = adjacent_operands[choice];
    cout << i << " " << j << endl;
    swap(E.expr[i], E.expr[j]);
}

void Info::M2_move(){
    if (operator_chains.empty()) return;

    int choice = rand() % operator_chains.size();
    auto[start, end] = operator_chains[choice];
    for(int i = start; i <= end; i++){
        E.expr[i].type = invert_type(E.expr[i].type);
    }
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

void Info::SA_algo(){
    cout << best_cost << endl;
    set_best_epression(best_cost);
    M3_move();
    long long current_cost = calculate_cost();
    if (current_cost < best_cost){
        set_best_epression(current_cost);
    }
    cout << current_cost << endl;
}
