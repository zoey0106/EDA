#pragma once
#include <iostream>
#include "DataTypes.hpp"
#include "BStarTree.hpp"
using namespace std;
/**
 * @brief The node structure of the HB*-tree
 */
/* Build Hierarchy Nodes */
template <typename T>
struct HierarchyNode{
    using ptr = unique_ptr<Node>;
    
    T x, y;
    T width, height;
    Node *lchild, *rchild;
};
/* Build Module Nodes */
struct ModuleNode{

};