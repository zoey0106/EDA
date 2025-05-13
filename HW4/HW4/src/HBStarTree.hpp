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
int64_t unique_id = 0;
template <typename T>
struct NodeBase{
    using ptr = unique_ptr<NodeBase>;

    enum class Kind {Regular, Contour, Hierarchy};
    T x, y;
    T width, height;
    Kind kind;
    ASFIsland *island; // Hierarchy node
    Node<T> *reg_node; // Regular node
    NodeBase *lchild, *rchild;
    NodeBase *parent;
    int64_t id;

    NodeBase(Kind k) : x(0), y(0), width(0), height(0), kind(k), island(nullptr), reg_node(nullptr), lchild(nullptr), rchild(nullptr), parent(nullptr) {
        id = unique_id++;
    }

    virtual ~NodeBase() = default;
public: 
    void setIsland(ASFIsland &isl) {island = &isl;}
    void setShape(T W, T H) {width = W; height = H;}
    void setPos (T X, T Y) {x = X; y = Y;}
    void setNode(Node<T> &n) {reg_node = &n;}
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
    node->setShape(island.tree.returnTotalWidth(), 0);

    /* Build left-skew contour node chain*/
    auto& top_contour = island.tree.getTopContour();
    NodeBase<T>* prev = nullptr;
    NodeBase<T>* first_contour = nullptr;

   for (const auto& contour : top_contour) {
        auto* contour_node = new NodeBase<T>(NodeBase<T>::Kind::Contour);
        contour_node->setPos(contour.x1, contour.y);
        contour_node->setShape(contour.x2 - contour.x1, 0); 

        if (!first_contour) first_contour = contour_node;
        if (prev) {
            prev->lchild = contour_node; 
            contour_node->parent = prev;
        }
        prev = contour_node;
    }
    node->rchild = first_contour;
    first_contour->parent = node;
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
    auto* BStarNode = new Node<T>;
    node->setNode(*BStarNode);
    blocks.ptr = BStarNode;
    blocks.ptr->setShape(blocks.width, blocks.height);
    return node;
}


/**
 * @brief A HB*-tree to calculate the coordinates of nodes and the area of the placement
 */
template <typename T>
class HBStarTree
{   
    std::unordered_map<NodeBase<T> *, int64_t> toInorderIdx;
    SegmentTree<T> contour_H;

    /* Builld tree */
    NodeBase<T> *buildTree(const std::vector<NodeBase<T> *> &preorder, const std::vector<NodeBase<T> *> &inorder, size_t &i, int64_t l, int64_t r){
        if (l > r || i >= preorder.size())
            return nullptr;
        NodeBase<T> *node = preorder[i++];
        assert(toInorderIdx.count(node) > 0 && "Node not found in inorder map.");
        int64_t idx = toInorderIdx[node];
        
        
        node->lchild = buildTree(preorder, inorder, i, l, idx - 1);
        if (node->lchild) node->lchild->parent = node;
        
        NodeBase<T>* rightSub = buildTree(preorder, inorder, i, idx + 1, r);

        if (node->kind == NodeBase<T>::Kind::Hierarchy && node->rchild) {
            NodeBase<T>* tail = node->rchild;
            while (tail->rchild) tail = tail->rchild;
            tail->rchild = rightSub;
            if (rightSub) rightSub->parent = tail;
        } else {
            node->rchild = rightSub;
            if (rightSub) rightSub->parent = node;
        }
        return node;
    }

    T getTotalWidth(NodeBase<T> *node) const
    {
        if (!node)
            return 0;

        return node->width + getTotalWidth(node->lchild) + getTotalWidth(node->rchild);
    }
    // void contour_updtate(NodeBase<T>* node){
    //     auto& top_contours = node->island->tree.getTopContour();
    //     NodeBase<ll> * contour = state.node->rchild;
    //     NodeBase<ll> * prev = contour;

    //     for (auto& top_c: top_contours){ // 如果有多 -> 建新node
    //         if (contour){
    //             contour->setPos(top_c.x1, top_c.y);
    //             contour->setShape(top_c.x2 - top_c.x1, 0);
    //             prev = contour;
    //             contour = contour->lchild;
    //         }
    //         else{
    //             auto* contour_node = new NodeBase<ll>(NodeBase<ll>::Kind::Contour);
    //             contour_node->setPos(top_c.x1, top_c.y);
    //             contour_node->setShape(top_c.x2 - top_c.x1, 0); 
    //             prev->lchild = contour_node;
    //             contour_node->parent = prev;
    //             prev = contour_node;
    //         }
    //     }
    //     vector<NodeBase<ll>*> dangling_nodes;
    //     vector<NodeBase<ll>*> del_contour_nodes;
    //     NodeBase<ll>* nearest_contour = prev;

    //     // 如果有少 刪node+搬移底下node
    //     while(contour){
    //         if (contour->rchild) dangling_nodes.push_back(contour->rchild);
    //         del_contour_nodes.push_back(contour);
    //         prev = contour;
    //         contour = contour->lchild;
    //     }
    //     // 刪contour node + 有關node的連結
    //     for (auto& node: del_contour_nodes){
    //         if (node->parent) node->parent->lchild = nullptr;
    //         node->parent = nullptr;
    //         node->lchild = nullptr;
    //         node->rchild = nullptr;
    //         delete node;
    //     }
    //     bool is_first = true;
    //     // 掛回 dangling node
    //     for (auto& dn: dangling_nodes){
    //         if (!dn) continue;
    //         if (is_first){
    //             // 往右看
    //             if (!nearest_contour->rchild){
    //                 nearest_contour->rchild = dn;
    //                 dn->parent = nearest_contour;
    //                 nearest_contour = nearest_contour->rchild;
    //                 is_first = false;
    //                 continue;
    //             } 
    //             nearest_contour = nearest_contour->rchild;
    //             is_first = false;
    //         }

    //         while(nearest_contour->lchild) nearest_contour = nearest_contour->lchild;
    //         nearest_contour->lchild = dn;
    //         dn->parent = nearest_contour;
    //         nearest_contour = nearest_contour->lchild;
    //     }
    // }
    /* Packing (get area) */
    void setPosition(NodeBase<T> *node, T startX){
        if (!node) return;

        if (node->kind == NodeBase<T>::Kind::Regular|| node->kind == NodeBase<T>::Kind::Contour){
            T endX = startX + node->width;
            T y = contour_H.query(startX, endX - 1); 
            contour_H.update(startX, endX - 1, y + node->height);
            node->setPos(startX, y);
            if (node->kind == NodeBase<T>::Kind::Regular){
                node->reg_node->x_abs = startX;
                node->reg_node->y_abs = y;
            }
            setPosition(node->lchild, endX);
            setPosition(node->rchild, startX);
        }
        else if (node->kind == NodeBase<T>::Kind::Hierarchy){
            auto& BottomContour = node->island->tree.getBottomContour();
            T max_y_contour = std::numeric_limits<T>::min(); 
            for (auto& contour: BottomContour){
                if (contour.y > max_y_contour ) max_y_contour = contour.y;
            }
            T max_y_add = std::numeric_limits<T>::min();
            T real_y = 0;
            for (auto& contour: BottomContour){
                T add_y = contour_H.query(startX + contour.x1, startX + contour.x2-1) + max_y_contour - contour.y;
                if (add_y > max_y_add) {
                    max_y_add = add_y; 
                    real_y = contour_H.query(startX + contour.x1, startX + contour.x2-1);
                }
            }

            T max_x = 0;
            for (auto& rep: node->island->reps){
                //testing
                rep.rep_node->setAbsPosition(startX, real_y);
                // 靠邀阿 rep node != block_node if self-sym
                if (rep.is_self) rep.right_block->ptr->setAbsPosition(startX, real_y);
                
                T lx = rep.rep_node->x_abs;
                T rx = lx + rep.right_block->width;
                T ty = rep.rep_node->y_abs + rep.right_block->height;
                
                if (ty > contour_H.query(lx, rx-1)) contour_H.update(lx, rx - 1, ty);
                max_x = std::max(rx, max_x);
                // non self-sym
                if (!rep.is_self){
                    rep.left_block->ptr->setAbsPosition(startX, real_y);
                    lx = rep.left_block->ptr->x_abs;
                    rx = lx + rep.left_block->ptr->width;
                    ty = rep.left_block->ptr->y_abs + rep.left_block->ptr->height;
                    if (ty > contour_H.query(lx, rx-1)) contour_H.update(lx, rx - 1, ty);
                    max_x = std::max(rx, max_x);
                }
            }
            //update contour
            // contour_update(node);//
            setPosition(node->lchild, max_x);
            setPosition(node->rchild, startX); //  contour
        }
    }
    void printTree(NodeBase<T> *n){
        if (!n) return;
        n->print_kind();
        printTree(n->lchild);
        printTree(n->rchild);
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
        contour_H.init(getTotalWidth(root));
        setPosition(root, 0);
    }

    /* Error checking */
    void printTree(){
        printTree(root);
    }
};
