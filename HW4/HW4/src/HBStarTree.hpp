// #pragma once
// #include <iostream>
// #include "DataTypes.hpp"
// #include "BStarTree.hpp"
// using namespace std;
// /**
//  * HB*tree's properties:
//  * 1. Hierarchy node 的左子若有，必不是contour node
//  * 2. Hierarchy node 的右子必有，且是contour node
//  * 3. contour node的左子若有，必是countour node代表下一個(右邊)的contour segment
//  * 4. contour node的右子若有，必不是countour node
//  * 5. regular node的左右子必非contour node
//  * 6. contour node的父輩不可以是regular node
//  */
// /**
//  * @brief The node structure of the HB*-tree
//  */
// /* Build Hierarchy Nodes */
// template <typename T>
// struct HierarchyNode{
//     using ptr = unique_ptr<Node>;
    
//     T x, y;
//     T width, height;
//     Node *lchild, *rchild;
// };
// /* Build Contour Nodes */
// struct ContourNode{

// };

// /* Build Regular Nodes */
// struct RegularNode{

// };