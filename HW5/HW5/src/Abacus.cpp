#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include "Abacus.hpp"
using namespace std;

ll overlap_site(Row& row, ll x){ //// 5 kinds of possible way 
    /** Return site num that x is on
     *  If no overlap -> return closest site_id
     */
    if (x <= row.x) return 0;
    ll site_id = (x - row.x) / row.site_width;
    if (site_id >= row.site.size()) return row.site.size() - 1;
    return site_id;
}

void AddCluster(Cluster& C1, Cluster& C2){
    // add C2 into C1
    C1.n_last = C2.n_last;
    C1.e_c += C2.e_c;
    C1.q_c += C2.q_c - C2.e_c * C1.s_w;
    C1.s_w += C2.s_w;
}

void Collapse(Row& row, vector<Cluster>& C, ll cur, ll prev){
    // Place Cluster C
    ll s_max = row.site.size() - 1;
    C[cur].s_id = C[cur].q_c / C[cur].e_c;
    if (C[cur].s_id < 0) C[cur].s_id = 0;
    if (C[cur].s_id > (s_max - C[cur].s_w + 1)) C[cur].s_id = s_max - C[cur].s_w + 1;

    if (prev >= 0) {
        // C & prev_C overlap
        if (C[prev].s_id + C[prev].s_w > C[cur].s_id){ 
            AddCluster(C[prev], C[cur]);
            C.erase(C.begin() + cur);
            Collapse(row, C, prev, prev - 1);
        }
    }

}

void AddCell(Cluster& C, Row& row, ll i, ll site_id){
    C.n_last = i;
    C.e_c += 1;
    C.q_c += site_id - C.s_w;
    C.s_w += (row.cells[i].width + row.site_width - 1)/ row.site_width;
}

ll PlaceRow(Row& row, ll D_max){
    /** Input: One row with N cells, x-pos of cells(global)
     *  Output: new(legal) x-pos of cells(xi) such that is not overlap and has min. displacement
     *  Return -1 if one of the cell in the row exceed max_D constriant
     *  Else return displacement (x - x')^2 + (y - y')^2 of cell 
     */
    vector<Cluster> C;

    for (ll i = 0; i < row.cells.size(); i++){
        ll cell_x = row.cells[i].x;
        ll cell_site = overlap_site(row, cell_x);
        // (!C.empty() && (C.back().s_id + C.back().s_w - 1) >= cell_site);
        bool is_overlap =  (!C.empty()) && (C.back().s_id + C.back().s_w - 1) >= cell_site; 

        if (i == 0 || !is_overlap){
            C.push_back(Cluster{i, 0, 0, 0, 0, cell_site});
            AddCell(C.back(), row, i, cell_site);
        }
        else {
            AddCell(C.back(), row, i, cell_site);
            Collapse(row, C, C.size() - 1, C.size() - 2);
        }
    }
    ll i = 0;
    double x_shift, y_shift;
    ll site_w, x;
    for (auto&c : C){
        x = c.s_id;
        while (i <= c.n_last){
            x_shift = row.cells[i].x - row.site[x].x;
            y_shift = row.cells[i].y - row.y;
            if (x_shift*x_shift + y_shift*y_shift > D_max) {
                // cout << "site id: " << x << "\n";
                // cout << "x: " << row.site[x].x << "y: " << row.y << endl;
                // cout << x_shift*x_shift + y_shift*y_shift << " " << D_max;
                // cout << "violate constraint\n" ;
                return -1;
            }// constraint
            site_w = (row.cells[i].width + row.site_width - 1)/ row.site_width;
            if ((x + site_w - 1)> row.site.size() - 1) return -1; // exceed row right boundary 
            row.cells[i].site_id = x;
            x += site_w;
            i ++;
        }
    }
    Cell& target_cell = row.cells.back();
    x_shift = target_cell.x - row.site[target_cell.site_id].x;
    y_shift = target_cell.y - row.y;
    return x_shift*x_shift + y_shift*y_shift;
}

ll LowerBound(Row& row, Cell& cell){
    double y_shift = row.y - cell.y;
    double x_shift = min (abs(row.x - cell.x), abs(row.x + row.width - cell.x));
    if (row.x <= cell.x && cell.x <= (row.x + row.width)) x_shift = 0;
    return x_shift * x_shift + y_shift * y_shift;
}

void Abacus(Info& data){
    vector<Row>& rows = data.rows;
    ll D_max = data.max_displaced * data.max_displaced;
    for (auto& cell: data.cells){
        ll ptr, Row_near = find_closest_row(rows, cell.y);
        ptr = Row_near;
        ll C_best = INT64_MAX;
        ll R_best = ptr;
        // Look UP
        while (ptr < rows.size()){
            Row row = rows[ptr];
            ll LB = LowerBound(row, cell); 
            if (LB < C_best){
                if (row.width < cell.width) {
                    ptr++;
                    continue;
                }
                row.cells.push_back(cell);
                // PLACEROW
                ll C_cur = PlaceRow(row, D_max);
                if (C_cur >= 0 && C_cur < C_best){
                    C_best = C_cur;
                    R_best = ptr;
                }
                row.cells.pop_back();
                ptr++;
            }
            else break;
        }
        ptr = Row_near - 1;
        // Look DOWN 
        while (ptr >= 0){
            Row row = rows[ptr];
            ll LB = LowerBound(row, cell);
            if (LB < C_best){
                if (row.width < cell.width) {
                    ptr--;
                    continue;
                }
                row.cells.push_back(cell);
                // PLACEROW
                ll C_cur = PlaceRow(row, D_max);
                if (C_cur >= 0 && C_cur < C_best){
                    C_best = C_cur;
                    R_best = ptr;
                }
                row.cells.pop_back();
                ptr--;
            }
            else break;
        }
        if (C_best == INT64_MAX) {
            cerr << "[Error] Cannot place cell: " << cell.name << endl;
            exit(1); 
        }
        rows[R_best].cells.push_back(cell);
        PlaceRow(rows[R_best], D_max);
    }
}