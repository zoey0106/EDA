#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include "DataTypes.hpp"

ll find_closest_row(vector<Row>& rows, dd y){
    /** Input DataType : Rows , double
     *  Return : Closest row index regarding y (first one)
     */

    auto it = lower_bound(rows.begin(), rows.end(), y, [&](const Row& row, dd val){return row.y < val;});

    if (it == rows.begin()) return 0;
    if (it == rows.end()) return rows.size()-1;

    size_t up = distance(rows.begin(), it);  
    size_t low = up - 1;
    return (abs(rows[up].y - y) < abs(rows[low].y - y)) ? up : low;
}

bool is_overlap(Blockage& blockage, Row& row){
    ll block_x1 = blockage.x;
    ll block_x2 = block_x1 + blockage.width;
    ll row_x1 = row.x;
    ll row_x2 = row_x1 + row.width;
    if (max(block_x1, row_x1) < min(block_x2, row_x2)) {
        ll block_y1 = blockage.y;
        ll block_y2 = block_y1 + blockage.height;
        ll row_y1 = row.y;
        ll row_y2 = row_y1 + row.height;
        if (max(block_y1, row_y1) < min(block_y2, row_y2)) return true;
    }
    return false;
}

void set_site_blcok(Row& row, ll x1, ll x2){
    ll site_id_start, site_id_end;
    if (row.x == x1) site_id_start = 0;
    else{
        site_id_start = (x1 - row.x) / row.site_width;
    }

    if ((row.x + row.width) == x2) site_id_end = row.site.size() - 1;
    else{
        site_id_end = (x2 - row.x) / row.site_width;
    }
    
    for (ll i = site_id_start; i <= site_id_end; i++){
        row.site[i].is_block = true;
    }
}

void set_block(Info& data){
    /** 
     *  Block site according to blockage
     */
    for (auto& blockage: data.blockages){
        for (auto& row: data.rows){
            if (is_overlap(blockage, row)){
                ll x1 = max(blockage.x, row.x);
                ll x2 = min(blockage.x + blockage.width, row.x + row.width);
                cout << "x1 :"<< x1 << " x2: " << x2 << endl;
                set_site_blcok(row, x1, x2);
            }
        }
    }
}

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
            cout << "site id: " << i++ << "  is_block: " << site.is_block << " x: " << site.x << " y:  " << site.y << "\n"; 
        }
    }
    cout << "----PRINT ROWS' site END----\n";
}