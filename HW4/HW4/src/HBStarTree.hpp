#pragma once
#include <iostream>
#include "DataTypes.hpp"
#include "BStarTree.hpp"
/**
 * HB*tree's properties:
 * 1. Hierarchy node 的左子若有，必不是contour node
 * 2. Hierarchy node 的右子必有，且是contour node
 * 3. contour node的左子若有，必是countour node代表下一個(右邊)的contour segment
 * 4. contour node的右子若有，必不是countour node
 * 5. regular node的左右子必非contour node
 * 6. contour node的父輩不可以是regular node
 */
/**
 * @brief The node structure of the HB*-tree
 */
template <typename T>
struct NodeBase{
    using ptr = unique_ptr<NodeBase>;

    enum class Kind {Regular, Contour, Hierarchy};
    T x, y;
    T width, height;
    Kind kind;
    ASFIsland *island; // Hierarchy node
    NodeBase *lchild, *rchild;

    NodeBase(Kind k) : x(0), y(0), width(0), height(0), kind(k), island(nullptr), lchild(nullptr), rchild(nullptr) {}

    virtual ~NodeBase() = default;
public: 
    void setIsland(ASFIsland &isl) {island = &isl;}
    void setShape(T W, T H) {width = W; height = H;}
    void setPos (T X, T Y) {x = X; y = Y;}
    void print_kind() {
        if (kind == Kind::Hierarchy) std::cout<< "Hierarchy node\n";
        if (kind == Kind::Regular) std::cout<< "Regular node\n";
        if (kind == Kind::Contour) std::cout<< "Contour node\n";
    }
};

template <typename T>
/* Build Heirarchy + contour chain node */
inline NodeBase<T>* build_heirarchy_contour_node(ASFIsland& island){
    /** Build Hierarchy node & chian its contour node on the left skew
     *  Input: ASF B*-tree Island 
     *  Return: Hierarchy node (root)
     */
    /* Build Hierarchy node*/
    auto* node = new NodeBase<T>(NodeBase<T>::Kind::Hierarchy);
    node->setIsland(island);

    /* Build left-skew contour node chain*/
    auto& top_contour = island.tree.getTopContour();
    NodeBase<T>* prev = nullptr;
    NodeBase<T>* first_contour = nullptr;

   for (const auto& contour : top_contour) {
        auto* contour_node = new NodeBase<T>(NodeBase<T>::Kind::Contour);
        contour_node->setPos(contour.x1, contour.y);
        contour_node->setShape(contour.x2 - contour.x1, 0); 

        if (!first_contour) first_contour = contour_node;
        if (prev) prev->lchild = contour_node; 
        prev = contour_node;
    }
    node->rchild = first_contour;
    return node;
}

template <typename T>
/* Build Regular node */
inline NodeBase<T>* build_regular_node(HardBlock& blocks){
    /** Build Regular node 
     *  Input: Non-symmetric HardBlock 
     *  Return: Regular node
     */
    auto* node = new NodeBase<T>(NodeBase<T>::Kind::Regular);
    node->setShape(blocks.width, blocks.height); 
    
    return node;
}


/**
 * @brief A HB*-tree to calculate the coordinates of nodes and the area of the placement
 */
template <typename T>
class HBStarTree
{   
    std::unordered_map<NodeBase<T> *, int64_t> toInorderIdx;
    SegmentTree<T> contourH;

    /* Builld tree */
    NodeBase<T> *buildTree(const std::vector<NodeBase<T> *> &preorder, const std::vector<NodeBase<T> *> &inorder, size_t &i, int64_t l, int64_t r){
        if (l > r || i >= preorder.size())
            return nullptr;
        NodeBase<T> *node = preorder[i++];
        assert(toInorderIdx.count(node) > 0 && "Node not found in inorder map.");
        int64_t idx = toInorderIdx[node];
        node->lchild = buildTree(preorder, inorder, i, l, idx - 1);
        while(node->rchild) node = node->rchild;
        node->rchild = buildTree(preorder, inorder, i, idx + 1, r);
        return node;
    }

    T getTotalWidth(Node<T> *node) const
    {
        if (!node)
            return 0;

        return node->width + getTotalWidth(node->lchild) + getTotalWidth(node->rchild);
    }

    /* Packing (get area) */
    void setPosition(NodeBase<T> *node, T startX){
        if (!node) return;

        if (node->kind == NodeBase::Kind::Regular){
            T endX = startX + node->width;
            T y = contourH.query(startX, endX - 1);
            contourH.update(startX, endX - 1, y + node->height);
            node->setPosition(startX, y);
            setPosition(node->lchild, endX);
            setPosition(node->rchild, startX);
        }
        else if (node->kind == NodeBase::Kind::Hierarchy){
            auto& bottom_contour = node->island->tree.bottom_contour;
            for (auto& contour: bottom_contour){
                
            }

        }
        else if (node->kind == NodeBase::Kind::Contour){

        }

    }

public:
    NodeBase<T> *root;
    HBStarTree() : root(nullptr) {}
    /* Builld tree */
    void buildTree(const std::vector<NodeBase<T> *> &preorder, const std::vector<NodeBase<T> *> &inorder) {
        assert(preorder.size() == inorder.size() && "The size of preorder and inorder must be the same.");
        int64_t n = inorder.size();

        toInorderIdx.clear();
        for (int64_t i = 0; i < n; ++i)
            toInorderIdx[inorder[i]] = i;

        size_t i = 0;
        root = buildTree(preorder, inorder, i, 0LL, n - 1);
    }
    /* Packing (get area) */
    void setPosition(){
        contourH.init(getTotalWidth(root));
        setPosition(root, 0);
    }
};
