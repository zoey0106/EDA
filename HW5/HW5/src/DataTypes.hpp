#pragma once
#include <string>
#include <cstdint>
#include <vector>
using namespace std;
typedef int64_t ll;
typedef double dd;
struct Cell {
    string name;
    ll width, height;
    dd x, y; // global placement result
    ll row_id, site_id; // detail placement result:
};

struct Blockage {
    string name;
    ll width, height;
    ll x, y; // global placement result
};

struct Site {
    ll x, y;
};

struct Row {
    string name;
    ll site_width;
    ll width, height;
    ll x, y; // global placement result
    vector<Site> site;
    vector<Cell> cells; // Init: empty
};

class Info {
public:
    Info() = default;
    vector<Cell> cells;
    vector<Blockage> blockages;
    vector<Row> rows;
    ll max_displaced;
    // ERROR check
    void print_cells();
    void print_blockages();
    void print_rows();
    void print_rows_by_name(string name);
    void print_site();
};

void sort(Info& data);
void set_subrow(Info& data);
ll find_closest_row(vector<Row>& rows, dd x, dd y);

class Cluster{
public:
    ll n_first, n_last;
    // s_id = x_c, s_w = w_c (# of site)
    ll q_c, e_c, s_w, s_id; 
};