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
    for (auto& group: data.sym_groups){
        island.push_back(build_ASF_BStar_Tree(data.hard_blocks, group));
    }

    /* Non-symmetry modules: Create module node*/
    // vector<Node<int64_t>*> non_sym;
    // for (auto& block: data.hard_blocks) {
    //     if (!block.is_sym) non_sym.push_back(block.ptr);
    // }
    // vector<Node<int64_t>*> pre, in;
    // make_left_skew_seq(non_sym, pre, in);
    // BStarTree<int64_t> bst;
    // bst.buildTree(pre, in);
    // bst.setPosition();

    // write_output(bst, data, argv[2]);

    return 0;
}