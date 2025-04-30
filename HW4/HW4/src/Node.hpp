#pragma once
#include <iostream>
#include "DataTypes.hpp"
#include "BStarTree.hpp"
using namespace std;
// inline: implement in .hpp and do not copy multimple copies
inline vector<Node<int64_t>*> build_nodes(vector<HardBlock>& hard_blocks){
    /* 
        Build nodes using blocks
        return: node vector 
    */
    vector<Node<int64_t>*> vec;
    vec.reserve(hard_blocks.size());

    for(auto& block: hard_blocks){
        if (block.height > block.width){
            swap(block.width, block.height);
            block.rotate = true; 
        } 
        auto* node_ptr = new Node<int64_t>;
        node_ptr->setShape(block.width, block.height);
        block.ptr = node_ptr;
        vec.push_back(node_ptr);
    }
    return vec;
}

inline void make_left_skew_seq(vector<Node<int64_t>*>& nodes, vector<Node<int64_t>*>& pre, vector<Node<int64_t>*>& in){
    pre = nodes;
    in = nodes;
}
