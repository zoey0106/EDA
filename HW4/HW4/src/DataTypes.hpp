#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include "BStarTree.hpp"
using namespace std;
struct HardBlock {
    string name;
    int64_t width;
    int64_t height;
    // B* tree
    Node<int64_t>* ptr = nullptr; // B* tree node
    bool rotate = false;
    bool is_sym = false;
};

enum class SymType {V, H};
struct SymPair {string id_1, id_2;};
struct SymSelf {string id;};

struct SymGroup {
    string name;
    SymType type = SymType::V;
    vector<SymPair> pairs;
    vector<SymSelf> selfs;
};

class Output {
    public:
        string name;
        int64_t x;
        int64_t y;
        bool rotate = false;
};

class Info {
    public:
        Info() = default;
    vector<HardBlock> hard_blocks;
    vector<SymGroup> sym_groups;
    vector<Output> output;
    int64_t best_area;
    // ERROR check
    void print_symgroup();
    void print_blocks();
};
