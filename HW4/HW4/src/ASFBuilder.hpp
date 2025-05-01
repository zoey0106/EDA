#pragma once
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include "DataTypes.hpp"
#include "BStarTree.hpp"
/* Build Represent-B* tree */
// Representative for each block in the sym. group
struct Represent {
    Node<int64_t>* rep_node; //B* node
    HardBlock* twin_block; //pick represent block
    bool is_self = false;//self or pair
};

inline vector<Represent> select_represent(vector<HardBlock>& blocks, SymGroup& group){
    /*
        input : one group
        return: picked representative for ASF-B* tree to build
    */
    unordered_map<string, HardBlock*> hard_block; 
    for (auto& block: blocks) hard_block[block.name] = &block;

    vector<Represent> reps;
    // pair: (s1, s1') -> pick s1'
    for (auto& pair: group.pairs){
        HardBlock* block = hard_block[pair.id_2];
        auto* node = new Node<int64_t>;
        node->setShape(block->width, block->height);
        reps.push_back({node, block, false});
    }
    // self: (s1) -> pick s1 and make its w/2 (h/2) if V (H)
    for (auto& self: group.selfs){
        HardBlock* block = hard_block[self.id];
        auto* node = new Node<int64_t>;
        if (group.type == SymType::V){
            node->setShape(block->width/2, block->height);
        }else if (group.type == SymType::H){
            node->setShape(block->width, block->height/2);
        }else cout << "ERROR in self in _select_represent_\n";
        reps.push_back({node, block, true});
    }
    return reps;
}

/* Build ASF-B* tree */
// Make sure correctness first
inline void build_ASF_seq_skew(vector<Represent>& reps, SymType type, vector<Node<int64_t>*>& pre, vector<Node<int64_t>*>& in){
    /*  
        Make pre/in seq. a skew B*tree & follow property 1
        If V: = right-most branch (Make self-rep the last in the seq.)
        If H: = left-most branch (Make self-rep the first in the seq.)
    */
    pre.clear(); in.clear();
    //pair
    for (auto& rep: reps){
        if (!rep.is_self){
            pre.push_back(rep.rep_node);
            in.push_back(rep.rep_node);
        }
    }
    //self
    for (auto& rep: reps){
        if (rep.is_self){
            pre.push_back(rep.rep_node);
            in.push_back(rep.rep_node);
        }
    }
    // if H
    if (type == SymType::H){
        reverse(pre.begin(), pre.end());
        reverse(in.begin(), in.end());
    }
}
// corresponding ASF-B*-tree for a group
struct ASFIsland {
    vector<Represent> reps; //representative in the same group
    BStarTree<int64_t> tree; //ASF-B*-tree
    int64_t axis; // for mirror
};

inline ASFIsland build_ASF_BStar_Tree(vector<HardBlock>& blocks, SymGroup& group){
    /* 
        Input: a group
        Return: ASF-B*-tree for the group 
    */
    vector<Represent> reps = select_represent(blocks, group);
    vector<Node<int64_t>*> pre, in;
    build_ASF_seq_skew(reps, group.type, pre, in);

    ASFIsland island{reps, {}, 0};

    island.tree.buildTree(pre, in);
    island.tree.setPosition();

    if (group.type == SymType::V){
        int64_t minX = numeric_limits<int64_t>::max();
        for (auto& r: reps) minX = min(minX, r.rep_node->x);
        island.axis = minX;
    }
    else if (group.type == SymType::H){
        int64_t minY = numeric_limits<int64_t>::max();
        for (auto& r: reps) minY = min(minY, r.rep_node->y);
        island.axis = minY;
    }

    return island;
}
