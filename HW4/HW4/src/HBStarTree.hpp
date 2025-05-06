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
// template <typename T>
// struct NodeBase{
//     using ptr = unique_ptr<NodeBase>;

//     enum class Kind {Regular, Contour, Hierarchy}
//     T x, y;
//     T width, height;
//     Kind kind;

//     NodeBase *lchild, *rchild;

//     NodeBase(Kind kind) : x(0), y(0), width(0), height(0), kind(k), lchild(nullptr), rchild(nullptr) {}

//     virtual ~NodeBase() = default;

//     void setShape(T w, T h) {width = w; height = h;}
//     void setPos (T X, T Y) {x = X; y = Y;}

// };
// /* Build Hierarchy Nodes */
// template <typename T>
// struct HierarchyNode : NodeBase<T>{
//     HierarchyNode() : NodeBase<T>(NodeBase<T>::Kind::Hierarchy) {}
// };
// /* Build Contour Nodes */
// template <typename T>
// struct ContourNode : NodeBase<T>{ 
//     ContourNode(T X, T Y, T W) : NodeBase<T>(NodeBase<T>::Kind::Contour){
//         this->setShape(W, 0); this->setPos(X, Y);
//     }
// };

// /* Build Regular Nodes */
// template <typename T>
// struct RegularNode : NodeBase<T>{
//     HardBlock* block;
//     RegularNode(HardBlock* b) : NodeBase<T>(NodeBase<T>::Kind::Regular), block(b) {}
// };

// inline void build_top_contour(ASFIsland& island, vector<contour_segment<int64_t>>& segments){
   
// }

// /* Build Heirarchy + contour chain node */
// inline void build_heirarchy_contour_node(ASFIsland& island){
//     vector<contour_segment<int64_t>> segments;
//     build_top_contour(island, segments);
// }

// /* Build Regular node */
// inline void build_regular_node(){

// }

// /* Build HB*-tree */
// inline void build_HBStarTree(){

// }