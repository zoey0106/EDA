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

// Area computation for hard blocks
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


// HPWL
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
                // add Area computation -> so x, y in block can be decide
            }
        }
    }


}

long long Info::calculate_cost(){
    

}

void Info::M1_move(){
    
}

void Info::SA_algo(){
    print_E();
    M1_move();
    print_E();
}
