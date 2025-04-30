#pragma once
#include <unordered_map>
#include <algorithm>
#include "DataTypes.hpp"
#include "BStarTree.hpp"
/* Build ASF-B* tree */
struct Represent {
    Node<int64_t>* rep_node;
    HardBlock* twin_block;
    bool is_pair;
};

inline vector<Represent> select_represent(const vector<HardBlock>& blocks, const SymGroup& g){
    /*
    input : one group
    return: representative for ASF-B* tree to build 
    */
    unordered_map<string, HardBlock*> block_list;
    for (auto& block: blocks) block_list[block.name] = const_cast<HardBlock*>(&block); 

    vector<Represent> reps;
    // pairs
    for (auto& p: g.pairs){
        auto* A = block_list[p.id_1];
        auto* B = block_list[p.id_2];
        // based on appear order
        bool chooseB = (B->name > A->name);
        HardBlock* rep = chooseB ? B:A;
        HardBlock* twin = chooseB ? A:B;
        reps.push_back({rep->ptr, twin, true});
    }
    // self
    for (auto& s: g.selfs) {
        HardBlock* block = block_list[s.id];
        reps.push_back({block->ptr, block, false});
    }
    return reps;
}

inline void build_ASF_seq(vector<Represent>& reps, SymType type, vector<Node<int64_t>*>& pre, vector<Node<int64_t>*>& in){
    /* Make self-rep the last in the seq.
    If V: = right-most branch
    If H: = left-most branch -> left&right mirror exchange
    */
    // pair
    for (auto& r: reps){
        if (r.is_pair){
            pre.push_back(r.rep_node);
            in.push_back(r.rep_node);
        }
    }
    // self
    for (auto& r:reps){
        if (!r.is_pair){
            pre.push_back(r.rep_node);
            in.push_back(r.rep_node);
        }
    }
    // If H-> self-rep need to be left-most
    if (type == SymType::H){
        reverse(pre.begin(), pre.end());
        reverse(in.begin(), in.end());
    }
}

struct ASFIsland {
    vector<Represent> reps; //representative in the same group
    BStarTree<int64_t> tree; //ASF-B*-tree
    int64_t axis; // for mirror
};

inline ASFIsland build_ASF_island(vector<HardBlock>& blocks, const SymGroup& g){
    auto reps = select_represent(blocks, g);
    vector<Node<int64_t>*> pre, in;
    build_ASF_seq(reps, g.type, pre, in);

    ASFIsland island{reps, {}, 0};
    island.tree.buildTree(pre, in);
    island.tree.setPosition();

    if (g.type == SymType::V){
        int64_t minX = numeric_limits<int64_t>::max();
        for (auto& r: reps) minX = min(minX, r.rep_node->x);
        island.axis = minX;
    }
    else if (g.type == SymType::H){
        int64_t minY = numeric_limits<int64_t>::max();
        for (auto& r: reps) minY = min(minY, r.rep_node->y);
        island.axis = minY;
    }

    return island;
}

inline void mirror_island(const ASFIsland& island, const SymGroup& g){
    int64_t axis = island.axis;
    for (auto& r: island.reps){
        auto* rep = r.rep_node;
        auto* twin = r.twin_block->ptr;

        if (r.is_pair){
            if (g.type == SymType::V){
                twin->x = 2*axis - (rep->x + rep->width);
                twin->y = rep->y;
            }else{
                twin->y = 2*axis - (rep->y + rep->height);
                twin->x = rep->x;
            }
        }// self
        else{
            if (g.type == SymType::V){
                twin->x = axis - rep->width / 2;
                twin->y = rep->y;
            }
            else{
                twin->y = axis - rep->height / 2;
                twin->x = rep->x;
            }
        }
    }
}