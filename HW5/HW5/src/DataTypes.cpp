#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <list>
#include "DataTypes.hpp"

// sort in ascending order
void sort_cell(Info& data){
    if (data.cells.empty()) throw runtime_error("vector [cells] empty!\n");

    sort(data.cells.begin(), data.cells.end(), [&](const Cell&a, const Cell& b){
        if (a.x == b.x) return a.y < b.y;
        return a.x < b.x;
    });
}

void sort_row(Info& data){
    if (data.rows.empty()) throw runtime_error("vector [rows] empty!\n");

    sort(data.rows.begin(), data.rows.end(), [&](const Row&a, const Row& b){
        if (a.y == b.y) return a.x < b.x;
        return a.y < b.y;
    });
}

void sort_blockage(Info& data){
    // if (data.blockages.empty()) throw runtime_error("vector [blockages] empty!\n");

    sort(data.blockages.begin(), data.blockages.end(), [&](const Blockage&a, const Blockage& b){
        if (a.y == b.y) return a.x < b.x;
        return a.y < b.y;
    });
}

void sort(Info& data){
    sort_cell(data);
    sort_row(data);
    sort_blockage(data);
}

ll find_closest_row(vector<Row>& rows, dd y){
    /** Input DataType : Rows , double y
     *  Return : Closest row index regarding y (first one)
     */

    auto it = lower_bound(rows.begin(), rows.end(), y, [&](const Row& row, dd val){return row.y < val;});

    if (it == rows.begin()) return 0; 
    if (it == rows.end()) return rows.size()-1; 

    size_t up = distance(rows.begin(), it);  
    size_t low = up - 1;
    return (abs(rows[up].y - y) < abs(rows[low].y - y)) ? up : low;
}

bool is_y_overlap(Blockage& blockage, Row& row){
    ll block_y1 = blockage.y;
    ll block_y2 = block_y1 + blockage.height;
    ll row_y1 = row.y;
    ll row_y2 = row_y1 + row.height;
    if (max(block_y1, row_y1) < min(block_y2, row_y2)) return true;
    return false;
}

bool is_x_overlap(Blockage& blockage, Row& row){
    ll block_x1 = blockage.x;
    ll block_x2 = block_x1 + blockage.width;
    ll row_x1 = row.x;
    ll row_x2 = row_x1 + row.width;
    if (max(block_x1, row_x1) < min(block_x2, row_x2)) return true;
    return false;
}

bool is_overlap(Blockage& blockage, Row& row){
    if (is_x_overlap(blockage, row)) return is_y_overlap(blockage, row);
    return false;
}

vector<Row> slice_row(Row& row, ll x1, ll x2){
    ll site_id_start, site_id_end;
    if (row.x == x1) site_id_start = 0;
    else{
        site_id_start = (x1 - row.x) / row.site_width;
    }

    if ((row.x + row.width) == x2) site_id_end = row.site.size() - 1;
    else{
        site_id_end = (x2 - row.x) / row.site_width;
        if (((x2 - row.x) % row.site_width) == 0) site_id_end -= 1;
        if (site_id_end < 0) site_id_end = 0;
    }

    vector<Site> vec1(row.site.begin(), row.site.begin() + site_id_start);// [0 ~ site_start - 1]
    vector<Site> vec2(row.site.begin() + site_id_end + 1, row.site.end());// [site_end + 1 ~ end]
    // update & create new row
    if (vec1.empty() && vec2.empty()) return {};
    vector<Row> rows;
    if (!vec1.empty()) {
        Row l_row{row.name, row.site_width, row.site_width * vec1.size(), row.height, vec1[0].x, row.y, vec1};
        rows.push_back(l_row);
    }
    if (!vec2.empty()){
        Row r_row{row.name, row.site_width, row.site_width * vec2.size(), row.height, vec2[0].x, row.y, vec2};
        rows.push_back(r_row);
    }
    return rows;
}

list<Row> build_row_list(Info& data){
    list<Row> row_list;
    for (auto& row: data.rows){
        row_list.push_back(row);
    }
    return row_list;
}

void set_subrow(Info& data){
    /** 
     *  Slice rows according to blockage into subrow
     */
    list<Row> row_list = build_row_list(data);
    auto row = row_list.begin();
    for (auto& block: data.blockages){
        while(row != row_list.end() && !is_y_overlap(block, *row)) ++row;
        auto ptr = row;
        while(is_y_overlap(block, *ptr)){
            if (is_x_overlap(block, *ptr)){
                ll x1 = max(block.x, ptr->x);
                ll x2 = min(block.x + block.width, ptr->x + ptr->width);
                vector<Row> sub_row = slice_row(*ptr, x1, x2);
                if (sub_row.size()) {
                    *ptr = sub_row[0];
                    if (sub_row.size() == 2) row_list.insert(next(ptr), sub_row[1]);
                    if (row == row_list.end()) break;
                    ++ptr;
                }
                else ptr = row_list.erase(ptr);
            }else{
                if (row == row_list.end()) break;
                ++ptr;
            }
        }
    }
    vector<Row> sliced_row;
    for (auto it = row_list.begin(); it != row_list.end(); ++it){
        sliced_row.push_back(*it);
    }
    data.rows = move(sliced_row);
    sort_row(data);
}

//Error check
void Info::print_cells(){
    cout << "----PRINT CELLS START----\n";
    for (auto& cell: cells){
        cout << "Cell Name: " <<cell.name << "  ";
        cout << "width: " << cell.width << "  " << "height: " << cell.height << "\n"; 
        cout << "global placement:   x: " << cell.x << "  y: " << cell.y << "\n";
    }
    cout << "----PRINT CELLS END----\n";
}

void Info::print_blockages(){
    cout << "----PRINT BLOCKAGES START----\n";
    for (auto& blockage: blockages){
        cout << "Blockage Name: " <<blockage.name << "\n";
        cout << "width: " << blockage.width << "  " << "height: " << blockage.height << "\n"; 
        cout << "x: " << blockage.x << "  y: " << blockage.y << "\n";
    }
    cout << "----PRINT BLOCKAGES END----\n";
}

void Info::print_rows(){
    cout << "----PRINT ROWS START----\n";
    for (auto& row: rows){
        cout << "Row Name: " <<row.name << "  ";
        cout << "width: " << row.width << "  " << "height: " << row.height << "\n"; 
        cout << "x: " << row.x << "  y: " << row.y << "\n";
        cout << "number of site: " << row.site.size() << "  site width:  " << row.site_width << "\n";
    }
    cout << "----PRINT ROWS END----\n";
}

void Info::print_site(){
    cout << "----PRINT ROWS' site START----\n";
    for (auto& row: rows){
        cout << "Row Name: " <<row.name << "  ";
        cout << "width: " << row.width << "  " << "height: " << row.height << "\n"; 
        cout << "x: " << row.x << "  y: " << row.y << "\n";
        cout << "number of site: " << row.site.size() << "  site width:  " << row.site_width << "\n";
        int i = 0;
        for (auto& site: row.site){
            cout << "site id: " << i++ << " x: " << site.x << " y:  " << site.y << "\n"; 
        }
    }
    cout << "----PRINT ROWS' site END----\n";
}