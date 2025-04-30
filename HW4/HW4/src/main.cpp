#include <iostream>
#include <fstream>
#include "DataTypes.hpp"
#include "Data_Loader.hpp"
#include "BStarTree.hpp"
#include "ASFBuilder.hpp"
#include "Node.hpp"
using namespace std;

int main(int argc, char *argv[]){
    Info data;
    ifstream input = read_file(argv[1]);
    build_data_structure(data, input);

    /* Symmetry Group: Construct ASF-B*-tree*/
    for (auto& g: data.sym_groups){
        ASFIsland island = build_ASF_island(data.hard_blocks, g);
        mirror_island(island, g);
        for (auto& r: island.reps){
            r.twin_block->is_sym = true;
        }
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