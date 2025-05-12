#pragma once
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include "DataTypes.hpp"
#include "BStarTree.hpp"
/**
 *  Build Represent-B* tree 
 *  Self-symmetric will have both Represent&Block node
 *  Pair-symmetric's block node = Represent node
 * */
// Representative for each block in the sym. group
struct Represent {
    Node<int64_t>* rep_node; //B* node
    HardBlock* right_block; //pick represent block
    HardBlock* left_block; // unpicked block
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
        HardBlock* block_id1 = hard_block[pair.id_1];
        HardBlock* block_id2 = hard_block[pair.id_2];
        auto* node = new Node<int64_t>;
        node->setShape(block_id2->width, block_id2->height);
        reps.push_back({node, block_id2, block_id1, false});
        block_id2->ptr = node;
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
        reps.push_back({node, block, block, true});
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
    SymType type = SymType::V;
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
    island.type = group.type;

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

inline void mirror_island(ASFIsland &island){
    /**
     * 1. Give island temporary coordinates 
     * 2. Create node for twin_blocks & self-sym block 
     * 3. Shift the x(y) to positive by adding max_x(max_y) of blocks if V(H)
     *  **I Do not mantain axis after cal. coordinate**
     * 4. Set is_sim = true
     * 5. ~set contour~
     * Accerlerate advice: less set_position
     */
    int64_t axis = island.axis;
    bool is_v = (island.type == SymType::V) ? true:false;
    int64_t max = INT64_MIN; // max_x(max_y) of blocks

    for (auto& rep: island.reps){
        HardBlock* block_id1 = rep.left_block;
        HardBlock* block_id2 = rep.right_block;
        int64_t x = rep.rep_node->x;
        int64_t y = rep.rep_node->y;
        int64_t tmp_max = is_v ? x + block_id2->width : y + block_id2->height;
        max = (tmp_max > max) ? tmp_max : max; 
        auto* node = new Node<int64_t>;
        node->setShape(block_id2->width, block_id2->height);
        if (!rep.is_self){ // pair
            // for twin block
            is_v ? node->setPosition(axis*2 - x - block_id2->width, y):node->setPosition(x, axis*2 - y - block_id2->height);
        }else if (rep.is_self){ // self
            //超容易錯 乾
            is_v ? node->setPosition(-block_id2->width/2 , y): node->setPosition(x, -block_id2->height/2);
        }
        block_id1->ptr = node;
    }
    // Shift coordinate to positive
    for (auto& rep: island.reps){
        HardBlock* block_id1 = rep.left_block;
        HardBlock* block_id2 = rep.right_block;
        Node<int64_t>* node_id1 = block_id1->ptr;
        Node<int64_t>* node_id2 = block_id2->ptr;

        if (is_v){
            block_id1->ptr->setPosition(node_id1->x + max, node_id1->y);
            if (!rep.is_self) block_id2->ptr->setPosition(node_id2->x + max, node_id2->y);
        }
        else if (!is_v){
            block_id1->ptr->setPosition(node_id1->x, node_id1->y + max);
            if (!rep.is_self) block_id2->ptr->setPosition(node_id2->x, node_id2->y + max);
        }
        block_id1->is_sym = true;
        block_id2->is_sym = true;
    }
    // build_contour before mirror
    island.tree.buildContours(is_v);

    // build_full_contour
    island.tree.mirrorContours(is_v, max);
}

// for perturbation
inline void set_island_position(ASFIsland &island){
    
    island.tree.setPosition();
    bool is_v = (island.type == SymType::V) ? true:false;

    if (is_v){
        int64_t minX = numeric_limits<int64_t>::max();
        for (auto& r: island.reps) minX = min(minX, r.rep_node->x);
        island.axis = minX;
    }
    else if (!is_v){
        int64_t minY = numeric_limits<int64_t>::max();
        for (auto& r: island.reps) minY = min(minY, r.rep_node->y);
        island.axis = minY;
    }

    int64_t axis = island.axis;
    int64_t max = INT64_MIN; // max_x(max_y) of blocks

    for (auto& rep: island.reps){
        HardBlock* block_id1 = rep.left_block;
        HardBlock* block_id2 = rep.right_block;
        int64_t x = rep.rep_node->x;
        int64_t y = rep.rep_node->y;
        int64_t tmp_max = is_v ? x + block_id2->ptr->width : y + block_id2->ptr->height;
        max = (tmp_max > max) ? tmp_max : max; 
        if (!rep.is_self){ // pair
            // for twin block
            is_v ? block_id1->ptr->setPosition(axis*2 - x - block_id2->ptr->width, y):block_id1->ptr->setPosition(x, axis*2 - y - block_id2->ptr->height);
        }else if (rep.is_self){ // self
            //超容易錯 乾
            is_v ? block_id1->ptr->setPosition(-block_id2->ptr->width/2 , y): block_id1->ptr->setPosition(x, -block_id2->ptr->height/2);
        }
    }
    // Shift coordinate to positive
    for (auto& rep: island.reps){
        HardBlock* block_id1 = rep.left_block;
        HardBlock* block_id2 = rep.right_block;
        Node<int64_t>* node_id1 = block_id1->ptr;
        Node<int64_t>* node_id2 = block_id2->ptr;

        if (is_v){
            block_id1->ptr->setPosition(node_id1->x + max, node_id1->y);
            if (!rep.is_self) block_id2->ptr->setPosition(node_id2->x + max, node_id2->y);
        }
        else if (!is_v){
            block_id1->ptr->setPosition(node_id1->x, node_id1->y + max);
            if (!rep.is_self) block_id2->ptr->setPosition(node_id2->x, node_id2->y + max);
        }
    }
    
    // build_contour before mirror
    island.tree.buildContours(is_v);

    // build_full_contour
    island.tree.mirrorContours(is_v, max);
}