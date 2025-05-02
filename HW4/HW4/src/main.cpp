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
    //Goal **: down contour !
    //Goal 2: make HB*tree
    //Goal 3: SA. algo.

    /* Symmetry Group: Construct ASF-B*-tree*/
    vector<ASFIsland> island; // ASF-B*-tree for each group
    for (auto& group: data.sym_groups){
        island.push_back(build_ASF_BStar_Tree(data.hard_blocks, group));
        mirror_island(island.back(), group);
    }
    
    /* Non-symmetry modules: Create module node*/
    //temp
    for (auto& block: data.hard_blocks) {
        if (!block.is_sym) {
            auto* node = new Node<int64_t>;
            node->setShape(block.width, block.height);
            node->setPosition(0, 7316);
            block.ptr = node;
        }
    }
    // vector<Node<int64_t>*> pre, in;
    // make_left_skew_seq(non_sym, pre, in);
    // BStarTree<int64_t> bst;
    // bst.buildTree(pre, in);
    // bst.setPosition();

    write_output(data, argv[2]);

    return 0;
}