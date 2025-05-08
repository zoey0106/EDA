#include <iostream>
#include <fstream>
#include "DataTypes.hpp"
#include "Data_Loader.hpp"
#include "BStarTree.hpp"
#include "ASFBuilder.hpp"
#include "HBStarTree.hpp"
using namespace std;

int main(int argc, char *argv[]){
    Info data;
    ifstream input = read_file(argv[1]);
    build_data_structure(data, input);
    //Goal 1: make B*tree follow property 1(make it the right most branch)-> now skew
    //Goal 2: make HB*tree 
    //Goal 3: SA. algo.

    /* Symmetry Group: Construct ASF-B*-tree*/
    vector<ASFIsland> island; // ASF-B*-tree for each group
    vector<NodeBase<int64_t>*> HB_node;
    for (auto& group: data.sym_groups){
        island.push_back(build_ASF_BStar_Tree(data.hard_blocks, group)); 
        mirror_island(island.back(), group);// contour
    }
    /* Create Hierarchy node + contour node chain */
    for (auto& isl: island){
        HB_node.push_back(build_heirarchy_contour_node<int64_t>(isl));
    }
    /* Create Regular node */
    for (auto& block: data.hard_blocks){
        if (!block.is_sym) HB_node.push_back(build_regular_node<int64_t>(block));
    } 
    // trivial squence
    vector<NodeBase<int64_t>*> pre = HB_node;
    vector<NodeBase<int64_t>*> in  = HB_node;
    
    HBStarTree<int64_t> HB_tree;
    HB_tree.buildTree(pre, in);
    HB_tree.setPosition();

    write_output(data, argv[2]);
    return 0;
}