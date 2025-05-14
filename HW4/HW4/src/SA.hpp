#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <array>
#include <chrono>
#include <unordered_set>
#include "Global_param.hpp"
#include "HBStarTree.hpp"
#include "ASFBuilder.hpp"

typedef int64_t ll;
/**
 * perturb 1: Node Movement
 * perturb 2: Module Rotation
 * perturb 3: Node Swapping
 * perturb 4: Representative Change
 * perturb 5: 
 */
class SA_Setting{
    public:
        SA_Setting() = default;
        // long long k = 10;
        // double eps = 1.0;
        // double r = 0.99;
        // double T = 1000.0;
        long long k = 5;
        double eps = 1.0;
        double r = 0.9;
        double T = 1000.0;
};

struct Record{
    // M3 (Hierarchy node)
    Node<int64_t>* node1;
    Node<int64_t>* node2;
    vector<ll> dangling_attach_contour; // 記第幾個，從0=第一個被刪node開始
    vector<NodeBase<ll>*> dangling_nodes;
    NodeBase<ll>* nearest_contour; // 沒被刪的最後一個

    // M3 (Regular)
    NodeBase<int64_t>*  node_swap;  // node_swap or node_attached
    // before-state 
    NodeBase<int64_t>*  oldParent_swap; // if null -> root
    bool                oldIsLeft_swap; 
    // after-state  
    NodeBase<int64_t>*  newParent_swap;
    bool                newIsLeft_swap;

    // M2 if pick Hierarchy node
    HardBlock* block1;
    HardBlock* block2; 
    bool is_self = false;
    // M1
    NodeBase<int64_t>*  node;  // Regular
    NodeBase<int64_t>* old_lchild;
    NodeBase<int64_t>* old_rchild;

    // before-state 
    NodeBase<int64_t>*  oldParent; // if null -> root
    bool                oldIsLeft; 
    // after-state  
    NodeBase<int64_t>*  newParent;
    bool                newIsLeft;
    Node<int64_t>* rep_node; 
    int M = 1; // movement
};

ll area(Info& data){
    ll maxX = 0, maxY = 0;
    for (const auto& hb : data.hard_blocks) {
        maxX = std::max(maxX, hb.ptr->x_abs + hb.ptr->width);
        maxY = std::max(maxY, hb.ptr->y_abs + hb.ptr->height);
    }
    return maxX*maxY;
}

inline double T_secheduling(double T, double r){
    return T * r;
}
// return 參數不確定這樣是什麼意思
inline NodeBase<int64_t>* pick_node(vector<NodeBase<int64_t>*>& HB_node){
    if (HB_node.empty()) return nullptr;
    uniform_int_distribution<size_t> dist(0, HB_node.size() - 1);
    return HB_node[dist(gen)];
}


inline void contour_update(Record& state){
    auto& top_contours = state.node->island->tree.getTopContour(); // new contour
    NodeBase<ll> * contour = state.node->rchild;
    NodeBase<ll> * prev = contour;

    for (auto& top_c: top_contours){ // 如果有多 -> 建新node
        if (contour){
            contour->setPos(top_c.x1, top_c.y);
            contour->setShape(top_c.x2 - top_c.x1, 0);
            prev = contour;
            contour = contour->lchild;
        }
        else{
            auto* contour_node = new NodeBase<ll>(NodeBase<ll>::Kind::Contour);
            contour_node->setPos(top_c.x1, top_c.y);
            contour_node->setShape(top_c.x2 - top_c.x1, 0); 
            prev->lchild = contour_node;
            contour_node->parent = prev;
            prev = contour_node;
        }
    }
    vector<NodeBase<ll>*> dangling_nodes;
    vector<NodeBase<ll>*> del_contour_nodes;
    NodeBase<ll>* nearest_contour = prev;
    state.nearest_contour = nearest_contour;

    // 如果有少 刪node+搬移底下node
    ll idx = 0;
    while(contour){
        if (contour->rchild){ 
            dangling_nodes.push_back(contour->rchild);
            state.dangling_attach_contour.push_back(idx);
        }
        idx++;
        del_contour_nodes.push_back(contour);
        prev = contour;
        contour = contour->lchild;
    }
    if (dangling_nodes.size()) state.dangling_nodes = dangling_nodes;

    // 刪contour node + 有關node的連結
    for (auto& node: del_contour_nodes){
        if (node->parent) node->parent->lchild = nullptr;
        node->parent = nullptr;
        node->lchild = nullptr;
        node->rchild = nullptr;
        delete node;
    }
    bool is_first = true;
    // 掛回 dangling node
    for (auto& dn: dangling_nodes){
        if (!dn) continue;
        if (is_first){
            // 往右看
            if (!nearest_contour->rchild){
                nearest_contour->rchild = dn;
                dn->parent = nearest_contour;
                nearest_contour = nearest_contour->rchild;
                is_first = false;
                continue;
            } 
            nearest_contour = nearest_contour->rchild;
            is_first = false;
        }

        while(nearest_contour->lchild) nearest_contour = nearest_contour->lchild;
        nearest_contour->lchild = dn;
        dn->parent = nearest_contour;
        nearest_contour = nearest_contour->lchild;
    }
}

inline void M1_rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    // 先拆原node
    auto* A = state.node;
    NodeBase<ll>* Ap = A->parent;
    NodeBase<ll>* Al = A->lchild;
    NodeBase<ll>* Ar = A->rchild;

    if (Ap) {
        state.newIsLeft ? Ap->lchild = Al: Ap->rchild = Ar;
        if (state.newIsLeft && Al) Al->parent = Ap;
        if (!state.newIsLeft && Ar) Ar->parent = Ap;
    }
    A->lchild = A->rchild = nullptr;

    if (state.old_lchild){
        A->lchild = state.old_lchild;
        A->lchild->parent = A;
    }
    if (state.old_rchild){
        A->rchild = state.old_rchild;

        if (state.old_rchild->parent) {
            (state.old_rchild->parent->lchild == state.old_rchild) ? state.old_rchild->parent->lchild = nullptr: state.old_rchild->parent->rchild = nullptr;
        }

        A->rchild->parent = A;
    }

    if (state.oldParent) {
        state.oldIsLeft ? state.oldParent->lchild = A : state.oldParent->rchild = A;
        A->parent = state.oldParent;
    } else {
        // root
        HB_tree.root = A;
        A->parent = nullptr;
    }

    HB_tree.setPosition(); // note
}

inline Record M1_move(HBStarTree<ll>& HB_tree, vector<NodeBase<ll>*>& HB_node){
    /**
     * ~Node Movement~
     * 1. pick regular node to move
     * 2. if attach_node is HB -> can only be placed at its left
     * 3. else placed either side.
     */
    /* Rollback infomation store */
    Record state{};
    state.M = 1;
    do state.node = pick_node(HB_node); 
    while(state.node->kind != NodeBase<ll>::Kind::Regular);
    
    state.oldParent = state.node->parent;
    if (state.oldParent){
        state.oldIsLeft = (state.oldParent->lchild == state.node);
    }
    state.old_rchild = state.node->rchild;
    state.old_lchild = state.node->lchild;
    
    do state.node_swap = pick_node(HB_node);
    while(state.node == state.node_swap);

    auto* A = state.node;
    auto* B = state.node_swap;
    NodeBase<ll>* Ap = A->parent;
    NodeBase<ll>* Al = A->lchild;
    NodeBase<ll>* Ar = A->rchild;
    // 1. 先決定要在新點下的左還是右
    uniform_int_distribution<size_t> dist(0, 1);
    state.newIsLeft = dist(gen);
    if (B->kind == NodeBase<ll>::Kind::Hierarchy) state.newIsLeft = true;
    // 2. 把點接過去 先切斷父母+小孩聯繫
    if (Ap) (state.oldIsLeft ? Ap->lchild : Ap->rchild) = nullptr;
    A->parent = A->lchild = A->rchild = nullptr;
    
    // 3. if 有小孩 
    if (Al) { // 有左孩
        if (Ap) (state.oldIsLeft ? Ap->lchild : Ap->rchild) = Al;
        else { // A is root -> Al become root
            HB_tree.root = Al;
        }
        Al->parent = Ap;
    }
    if (Ar) { // 有右+左孩 -> 找左孩的最右點 / 若沒左孩+A是root 自己當root / 沒左孩 -> 直接自己接
        if (Al){
            while(Al->rchild) Al = Al->rchild;
            Al->rchild = Ar;
            Ar->parent = Al;
        }
        else{
            if (Ap) (state.oldIsLeft ? Ap->lchild : Ap->rchild) = Ar;
            else {
                HB_tree.root = Ar;
            }
            Ar->parent = Ap;
        }
    }
    NodeBase<ll>* Bl = B->lchild;
    NodeBase<ll>* Br = B->rchild;
    state.newIsLeft ? A->lchild = Bl: A->rchild = Br;
    
    if (state.newIsLeft) {
        if (Bl) Bl->parent = A;
    } else {
        if (Br) Br->parent = A;
    }     
    state.newIsLeft ? B->lchild = A : B->rchild = A; // 父母接小孩
    A->parent = B;

    HB_tree.setPosition(); // cal. new coor.
    return state;
}

inline void pick_blk(Record& state, vector<Represent>& reps){
    uniform_int_distribution<size_t> dist(0, reps.size()-1);
    int idx = dist(gen);
    state.block1 = reps[idx].right_block;
    if (!reps[idx].is_self) state.block2 = reps[idx].left_block;
    state.is_self = reps[idx].is_self;
    state.rep_node = reps[idx].rep_node;
}

inline void M3_regular_swap(Record& state, HBStarTree<ll>& HB_tree){
    auto* A = state.node;
    auto* B = state.node_swap;
    if (!A || !B || A == B) return;
    NodeBase<ll>* Ap = A->parent;
    NodeBase<ll>* Al = A->lchild;
    NodeBase<ll>* Ar = A->rchild;

    NodeBase<ll>* Bp = B->parent;
    NodeBase<ll>* Bl = B->lchild;
    NodeBase<ll>* Br = B->rchild;
    bool A_is_left = (Ap && Ap->lchild == A);
    bool B_is_left = (Bp && Bp->lchild == B);

    // detach from parent
    if (Ap) (A_is_left ? Ap->lchild : Ap->rchild) = nullptr;
    if (Bp) (B_is_left ? Bp->lchild : Bp->rchild) = nullptr;
    // detach its childs
    A->lchild = A->rchild = nullptr;
    B->lchild = B->rchild = nullptr;

    // connect parent to A/B
    A->parent = Bp;
    if (Bp) (B_is_left ? Bp->lchild : Bp->rchild) = A;
    else     HB_tree.root = A;                // B 原本是 root
    B->parent = Ap;
    if (Ap) (A_is_left ? Ap->lchild : Ap->rchild) = B;
    else     HB_tree.root = B;                // A 原本是 root
    // swap A/B
    A->lchild = ((Bl != A) ? Bl:B);   
    if (Bl && (Bl != A)) Bl->parent = A;
    if ((Bl == A) && B) B->parent = A;
    A->rchild = ((Br != A) ? Br:B);   
    if (Br && (Br != A)) Br->parent = A;
    if ((Br == A) && B) B->parent = A;

    B->lchild = ((Al != B) ? Al:A);   
    if (Al && (Al != B)) Al->parent = B;
    if ((Al == B) && A) A->parent = B;
    B->rchild = ((Ar != B) ? Ar:A);   
    if (Ar && (Ar != B)) Ar->parent = B;
    if ((Ar == B) && A) A->parent = B;
}


inline void pick_two_node(Record& state, vector<Represent>& reps){
    // 挑選內部node時 self-sym 只能在左斜或右斜 (先不挑self-sym)
    vector<ll> node_idx; // only non-self-sym
    node_idx.reserve(reps.size());
    
    for (ll i = 0; i < reps.size(); i++){
        if (!reps[i].is_self) node_idx.push_back(i);
    }
    if (node_idx.size() < 2) {
        state.node1 = state.node2 = nullptr;
        return;
    }
    array<ll, 2> idx{};
    sample(node_idx.begin(), node_idx.end(), idx.begin(), 2, gen);
    state.node1 = reps[idx[0]].rep_node;
    state.node2 = reps[idx[1]].rep_node;
    // cout << "Swap: " << reps[idx[0]].right_block->name << "  ";
    // cout << "Swap: " << reps[idx[1]].right_block->name << endl;
}

inline void M3_hierarchy_swap(Record& state, BStarTree<ll>& B_tree){
    auto* A = state.node1;
    auto* B = state.node2;
    
    if (!A || !B || A == B) return;
    Node<ll>* Ap = A->parent;
    Node<ll>* Al = A->lchild;
    Node<ll>* Ar = A->rchild;

    Node<ll>* Bp = B->parent;
    Node<ll>* Bl = B->lchild;
    Node<ll>* Br = B->rchild;
    bool A_is_left = (Ap && Ap->lchild == A);
    bool B_is_left = (Bp && Bp->lchild == B);

    // detach from parent
    if (Ap) (A_is_left ? Ap->lchild : Ap->rchild) = nullptr;
    if (Bp) (B_is_left ? Bp->lchild : Bp->rchild) = nullptr;
    // detach its childs
    A->lchild = A->rchild = nullptr;
    B->lchild = B->rchild = nullptr;

    // connect parent to A/B
    A->parent = Bp;
    if (Bp) (B_is_left ? Bp->lchild : Bp->rchild) = A;
    else     B_tree.root = A;                // B 原本是 root
    B->parent = Ap;
    if (Ap) (A_is_left ? Ap->lchild : Ap->rchild) = B;
    else     B_tree.root = B;                // A 原本是 root
    // swap A/B
    A->lchild = ((Bl != A) ? Bl:B);   
    if (Bl && (Bl != A)) Bl->parent = A;
    if ((Bl == A) && B) B->parent = A;
    A->rchild = ((Br != A) ? Br:B);   
    if (Br && (Br != A)) Br->parent = A;
    if ((Br == A) && B) B->parent = A;

    B->lchild = ((Al != B) ? Al:A);   
    if (Al && (Al != B)) Al->parent = B;
    if ((Al == B) && A) A->parent = B;
    B->rchild = ((Ar != B) ? Ar:A);   
    if (Ar && (Ar != B)) Ar->parent = B;
    if ((Ar == B) && A) A->parent = B;
}

inline Record M3_node_swapping(HBStarTree<ll>& HB_tree, vector<NodeBase<ll>*>& HB_node){
    // can only swap same kind of node 
    Record state{};
    // state.node = pick_node(HB_node); 
    state.node = pick_node(HB_node);
    state.M = 3;
    state.oldParent = state.node->parent;
    if (state.oldParent){
        state.oldIsLeft = (state.oldParent->lchild == state.node);
    }
    if (state.node->kind == NodeBase<ll>::Kind::Regular){
        // 只跟同樣為regular的人swap
        do state.node_swap = pick_node(HB_node);
        while (state.node->parent == state.node_swap || state.node_swap->parent == state.node);
        state.oldParent_swap = state.node_swap->parent;
        if (state.oldParent_swap){
            state.oldIsLeft_swap = (state.oldParent_swap->lchild == state.node_swap);
        }
        if (state.node_swap->kind == NodeBase<ll>::Kind::Regular){
            M3_regular_swap(state, HB_tree);
        }
        else {
            state.node = state.node_swap; // 改成內部swap
            state.oldParent = state.oldParent_swap;
            if (state.oldParent){
                state.oldIsLeft = state.oldIsLeft_swap;
            }
        }
    }
    if (state.node->kind == NodeBase<ll>::Kind::Hierarchy){
        // swap 內部node
        pick_two_node(state, state.node->island->reps);
        M3_hierarchy_swap(state, state.node->island->tree); // 有dangling node -> note
        set_island_position(*state.node->island);
        contour_update(state); // set relative coor. to contour & update contour node num
    }
    HB_tree.setPosition();
    return state;
}

inline Record M2_move_rotation(HBStarTree<ll>& HB_tree, vector<NodeBase<ll>*>& HB_node){
    Record state{};
    state.node = pick_node(HB_node); 
    state.M = 2;
    if (state.node->kind == NodeBase<ll>::Kind::Hierarchy){
        pick_blk(state, state.node->island->reps); // & set state
        swap(state.block1->ptr->width, state.block1->ptr->height); // swap Node 的w,h
        state.block1->ptr->rotate ^= 1;
        if (state.is_self && state.node->island->type == SymType::V){
            swap(state.rep_node->width, state.rep_node->height);
            state.rep_node->width /= 2;
            state.rep_node->height *= 2;
        }
        else if (state.is_self && state.node->island->type == SymType::H){
            swap(state.rep_node->width, state.rep_node->height);
            state.rep_node->width *= 2;
            state.rep_node->height /= 2;
        }

        if (!state.is_self){
            swap(state.block2->ptr->width, state.block2->ptr->height);
            state.block2->ptr->rotate ^= 1;
        } 
        // 需set 內部的position!
        set_island_position(*state.node->island);
        // contour node updates
        contour_update(state);
    }
    else if (state.node->kind == NodeBase<ll>::Kind::Regular){
        state.node->reg_node->rotate ^= 1;
        swap(state.node->reg_node->width, state.node->reg_node->height); // swap Node 的w,h
        swap(state.node->width, state.node->height); // swap NodeBase 的w,h 
    }
    
    HB_tree.setPosition();

    return state;
}

inline void node_swapback(Record state){
    int idx = 0;
    int ptr = 0;
    NodeBase<ll>* cur_node = state.nearest_contour->lchild;
    // not tested yet
    for (auto& node: state.dangling_nodes){
        cout << "node_swap\n";
        while(idx != state.dangling_attach_contour[ptr]){
            cur_node = cur_node->lchild;     
            idx++;   
        }
        (node->parent->lchild == node)? node->parent->lchild = nullptr: node->parent->rchild = nullptr;
        cur_node->rchild = node;
        node->parent = cur_node;
        ptr++;
    }
}

inline void M3_rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    if (state.node->kind == NodeBase<ll>::Kind::Regular){
        M3_regular_swap(state, HB_tree);
    }
    else {
        M3_hierarchy_swap(state, state.node->island->tree);
        set_island_position(*state.node->island);
        contour_update(state);
        node_swapback(state);// 尚未確認
    }
    HB_tree.setPosition();
}

inline void M2_rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    if (state.node->kind == NodeBase<ll>::Kind::Hierarchy){
        swap(state.block1->ptr->width, state.block1->ptr->height); // swap Node 的w,h
        state.block1->ptr->rotate ^= 1;
        if (state.is_self){
            swap(state.rep_node->width, state.rep_node->height);
            state.rep_node->width /= 2;
            state.rep_node->height *= 2;
        } 
        else if (state.is_self && state.node->island->type == SymType::H){
            swap(state.rep_node->width, state.rep_node->height);
            state.rep_node->width *= 2;
            state.rep_node->height /= 2;
        }
        if (!state.is_self){
            swap(state.block2->ptr->width, state.block2->ptr->height);
            state.block2->ptr->rotate ^= 1;
        } 
        // 需set 內部的position!
        set_island_position(*state.node->island);
        contour_update(state);
    }
    else if (state.node->kind == NodeBase<ll>::Kind::Regular){
        state.node->reg_node->rotate ^= 1;
        swap(state.node->reg_node->width, state.node->reg_node->height); // swap Node 的w,h
        swap(state.node->width, state.node->height); // swap NodeBase 的w,h 
    }
    HB_tree.setPosition();
}

inline void swap_subtree(Node<ll> *node){
    if (!node) return ;
    Node<ll> *lchild_temp = node->lchild;
    node->lchild = node->rchild;
    node->rchild = lchild_temp;
    swap_subtree(node->lchild);
    swap_subtree(node->rchild);
}

inline void M4_rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    // rotate every module
    for (auto& rep: state.node->island->reps){
        swap(rep.right_block->ptr->width, rep.right_block->ptr->height);
        rep.right_block->ptr->rotate ^= 1;
        if (rep.is_self) {
            swap(rep.rep_node->width, rep.rep_node->height);
        }
        if (!rep.is_self){
            swap(rep.left_block->ptr->width, rep.left_block->ptr->height);
            rep.left_block->ptr->rotate ^= 1;
        }
    }
    // swap every lchild and rchild (full subtree swap)
    swap_subtree(state.node->island->tree.root);
    // change symmetirc type
    if (state.node->island->type == SymType::V) state.node->island->type = SymType::H;
    else state.node->island->type = SymType::V;
    // 需set 內部的position!
    set_island_position(*state.node->island);
    // contour node updates
    contour_update(state);
    HB_tree.setPosition();
}

inline Record M4_move_convert_sym_type(HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    // self-sym 有問題
    Record state{};
    do state.node = pick_node(HB_node);
    while(state.node->kind == NodeBase<ll>::Kind::Regular);
    state.M = 4;
    // rotate every module
    for (auto& rep: state.node->island->reps){
        rep.right_block->ptr->rotate ^= 1;
        swap(rep.right_block->ptr->width, rep.right_block->ptr->height);
        if (rep.is_self) {
            swap(rep.rep_node->width, rep.rep_node->height);
        }
        if (!rep.is_self){
            swap(rep.left_block->ptr->width, rep.left_block->ptr->height);
            rep.left_block->ptr->rotate ^= 1;
        }
    }
    // swap every lchild and rchild (full subtree swap)
    swap_subtree(state.node->island->tree.root);
    // change symmetirc type
    if (state.node->island->type == SymType::V) state.node->island->type = SymType::H;
    else state.node->island->type = SymType::V;

    // 需set 內部的position!
    set_island_position(*state.node->island);
    // contour node updates
    contour_update(state);
    HB_tree.setPosition();
    return state;
}

inline void rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    switch(state.M){
        case 1: M1_rollback(state, HB_tree, HB_node); 
            break;
        case 2: M2_rollback(state, HB_tree, HB_node);
            break;
        case 3: M3_rollback(state, HB_tree, HB_node);
            break;
        case 4: M4_rollback(state, HB_tree, HB_node);
            break;
        default: break;
    }
}

inline Record select_move(HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    uniform_real_distribution<double> dist(0.0, 1.0);
    double prob = dist(gen);
    
    if (prob > 0.8) {
        return M1_move(HB_tree, HB_node);
    }
    else if (prob > 0.6) {
        return M2_move_rotation(HB_tree, HB_node);
    }
    else if (prob > 0.01){
        return M3_node_swapping(HB_tree, HB_node);
    }
    // else return M4_move_convert_sym_type(HB_tree, HB_node);

    return M1_move(HB_tree, HB_node);
}

inline void set_best_floorplan(Info& data){
    int idx = 0;
    for (const auto& hb: data.hard_blocks){
        data.output[idx].rotate = hb.ptr->rotate;
        data.output[idx].x = hb.ptr->x_abs;
        data.output[idx].y = hb.ptr->y_abs;
        idx++;
    }
    data.best_area = area(data);
}

inline void SA_algo(SA_Setting setting, Info& data, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    //setting.reset();
    double MT = 0, reject = 0, T = setting.T;
    int uphill = 0, N = setting.k * data.hard_blocks.size();
    ll best_cost = area(data);
    ll prev_cost = best_cost;

    cout << "Initial best area: " << best_cost << "\n";

    /* store best sol. in x/y coor. */
    set_best_floorplan(data);
    
    while((reject/(MT == 0 ? 1 : MT)) <= 0.95 && T >= setting.eps){
        //--------time check--------//
        auto now = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration<double>(now - program_start).count();
        if (elapsed > TIME_LIMIT) {
            cout << "[Info] Exiting due to TIME_LIMIT\n";
            break;
        }
        //--------time check--------//
        MT = 0, reject = 0, uphill = 0;
        while(uphill <= N && MT <= 2*N){
            //--------time check--------//
            auto now = chrono::high_resolution_clock::now();
            double elapsed = chrono::duration<double>(now - program_start).count();
            if (elapsed > TIME_LIMIT) {
                cout << "[Info] Exiting due to TIME_LIMIT\n";
                break;
            }
            //--------time check--------//
            Record state = select_move(HB_tree, HB_node);
            ll cur_cost = area(data);
            ll delta_c = cur_cost - prev_cost;
            uniform_real_distribution<double> dist(0.0, 1.0);
            MT++;
            if (delta_c <= 0){// accept
                prev_cost = cur_cost;
                if (cur_cost < best_cost){
                    set_best_floorplan(data);
                    best_cost = cur_cost;
                }
            }
            else{
                double random = dist(gen);
                if (random < exp(-delta_c/T)){// accept
                    uphill++;
                    prev_cost = cur_cost;
                }
                else{// reject
                    reject++;
                    rollback(state, HB_tree, HB_node);
                }
            }
        }
        T = T_secheduling(T, setting.r);
        cout << "Last cost:  " << prev_cost << " [ T: " << T << " ]\n";
    }

    cout << "Final area: " << data.best_area << "\n";
}