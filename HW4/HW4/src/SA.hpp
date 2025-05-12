#pragma once
#include <string>
#include <vector>
#include <chrono>
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
        long long k = 5;
        double eps = 1.0;
        double r = 0.9;
        double T = 1500.0;
        void reset(){
            k = 5; 
            eps = 1.0;
            r = 0.9;
            T = 1500.0;
        };
};

struct Record{
    // M3 (Hierarchy node)
    
    // M3 (Regular)
    NodeBase<int64_t>*  node_swap;  
    // before-state 
    NodeBase<int64_t>*  oldParent_swap; // if null -> root
    bool                oldIsLeft_swap; 
    NodeBase<int64_t>*  oldSibling_swap; 
    // after-state  
    NodeBase<int64_t>*  newParent_swap;
    bool                newIsLeft_swap;

    // M2 if pick Hierarchy node
    HardBlock* block1;
    HardBlock* block2; 
    bool is_self = false;
    // M1
    NodeBase<int64_t>*  node;  
    // before-state 
    NodeBase<int64_t>*  oldParent; // if null -> root
    bool                oldIsLeft; 
    NodeBase<int64_t>*  oldSibling; 
    // after-state  
    NodeBase<int64_t>*  newParent;
    bool                newIsLeft;

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
// 先不寫有root的情況
// 挑到attach node的時候不能祖先有自己 -> 無限遞迴

// inline void M1_move_regular_node(Record& state, NodeBase<int64_t>* attached_node){
//     bool is_root = (state.oldParent) ? false:true; // no parent = root
//     bool is_empty_l = (attached_node->rchild) ? true:false; // 會傾向接右邊
    
//     // 自己父母改掉
//     (is_empty_l) ? attached_node->lchild = state.node: attached_node->rchild = state.node;
//     state.node->parent = attached_node;
//     // 建Record
//     state.newParent = attached_node;
//     state.newIsLeft = is_empty_l;

//     if (state.node->rchild && state.node->lchild){ // 2 child
//         // 把左小孩接給父母+右小孩接另一邊最右(最左)
//         (state.oldIsLeft) ? state.oldParent->lchild = state.node->lchild: state.oldParent->rchild = state.node->lchild;
//         state.node->lchild->parent = state.oldParent;

//         if (state.oldIsLeft){ // 右孩要接到最右邊
//             NodeBase<int64_t>* r_most_node = state.oldParent->rchild;
//             while(r_most_node->rchild) r_most_node = r_most_node->rchild;
//             r_most_node->rchild = state.node->rchild;
//             state.node->rchild->parent = r_most_node;
//         }
//         else{ // 右孩要接到最左邊
//             NodeBase<int64_t>* l_most_node = state.oldParent->lchild;
//             while(l_most_node->lchild) l_most_node = l_most_node->lchild;
//             l_most_node->lchild = state.node->rchild;
//             state.node->rchild->parent = l_most_node;
//         }
//     }   
//     else if (state.node->rchild || state.node->lchild){ // 1 child
//         bool is_r = (state.node->rchild) ? true:false; 
//         // 把小孩接給父母
//         if (state.oldIsLeft) {
//             (is_r) ? state.oldParent->lchild = state.node->rchild :state.oldParent->lchild = state.node->lchild;
//             (is_r) ? state.node->rchild->parent = state.oldParent :state.node->lchild->parent = state.oldParent;
//         }
//         else {
//             (is_r) ? state.oldParent->rchild = state.node->rchild :state.oldParent->rchild = state.node->lchild;
//             (is_r) ? state.node->rchild->parent = state.oldParent :state.node->lchild->parent = state.oldParent;
//         }
//     }
//     else if (!state.node->rchild && !state.node->lchild){ // leaf
//         (state.oldIsLeft) ? state.oldParent->lchild = nullptr: state.oldParent->rchild = nullptr; 
//     }

// }

// inline Record M1_move(HBStarTree<ll>& HB_tree, vector<NodeBase<ll>*>& HB_node){
//     /**
//      * ~Node Movement~
//      * 1. pick HB node / regular node to move
//      * 2. contour node followed HB_node (can't be picked)
//      * 3. if first is HB_node then it can picked node inside as well (inside swap)
//      */
//     /* Rollback infomation store */
//     Record state{};

//     do state.node = pick_node(HB_node); 
//     while(!state.node->parent || state.node->kind != NodeBase<ll>::Kind::Regular);

//     if (state.node == nullptr) cout << "M1: picked node ERROR!\n";
    
//     state.oldParent = state.node->parent;
//     if (state.oldParent){
//         state.oldIsLeft = (state.oldParent->lchild == state.node);
//         state.oldSibling = state.oldIsLeft ? state.oldParent->rchild : state.oldParent->lchild;
//     }
    
//     NodeBase<int64_t>* node = pick_node(HB_node);
//     while(node == state.node || (node->rchild && node->lchild)) node = pick_node(HB_node); // 必有一邊無子 -> 可能挑到要去root下面，但沒關西，有nullptr

//     bool is_HB = (node->kind == NodeBase<ll>::Kind::Hierarchy) ? true:false;

//     if (state.node->kind == NodeBase<ll>::Kind::Regular){
//         M1_move_regular_node(state, node);
//     }
//     // else if (state.node->kind == NodeBase<ll>::Kind::Hierarchy){

//     // }
//     // else{

//     // }
//     HB_tree.setPosition(); // cal. new coor.
//     return state;
// }

inline void pick_blk(Record& state, vector<Represent>& reps){
    uniform_int_distribution<size_t> dist(0, reps.size()-1);
    int idx = dist(gen);
    state.block1 = reps[idx].right_block;
    if (!reps[idx].is_self) state.block2 = reps[idx].left_block;
    state.is_self = reps[idx].is_self;
}

// inline void M3_regular_swap(Record& state, HBStarTree<ll>& HB_tree){
//     // 1. temp自己小孩+父母
//     NodeBase<ll>* node_lchild = state.node->lchild;
//     NodeBase<ll>* node_rchild = state.node->rchild;
//     NodeBase<ll>* node_parent = state.node->parent;
//     NodeBase<ll>* node_swap_lchild = state.node_swap->lchild;
//     NodeBase<ll>* node_swap_rchild = state.node_swap->rchild;
//     NodeBase<ll>* node_swap_parent = state.node_swap->parent;
//     // 2. 改自己小孩跟父母 if 右(左)孩或父親=自己-> 右(左)孩或父親=對方
//     (node_swap_lchild != state.node) ? state.node->lchild = node_swap_lchild:state.node->lchild = state.node_swap;
//     (node_swap_rchild != state.node) ? state.node->rchild = node_swap_rchild:state.node->rchild = state.node_swap;
//     (node_swap_parent != state.node) ? state.node->parent = node_swap_parent:state.node->parent = state.node_swap;
//     // 3. 改對方小孩跟父母 if 右(左)孩或父親=自己-> 右(左)孩或父親=對方
//     (node_lchild != state.node_swap) ? node_swap_lchild = node_lchild:node_swap_lchild = state.node;
//     (node_rchild != state.node_swap) ? node_swap_rchild = node_rchild:node_swap_rchild = state.node;
//     (node_parent != state.node_swap) ? node_swap_parent = node_parent:node_swap_parent = state.node;
//     // 4. 改雙方小孩的父母
//     if (node_swap_lchild && node_swap_lchild != state.node) node_swap_lchild->parent = state.node;
//     else if (node_swap_lchild && node_swap_lchild == state.node) state.node_swap->parent = state.node;
        
//     if (node_swap_rchild && node_swap_rchild != state.node) node_swap_rchild->parent = state.node;
//     else if (node_swap_rchild && node_swap_rchild == state.node) state.node_swap->parent = state.node;
    
//     if (node_swap_parent != state.node && node_swap_parent){
//         (state.oldIsLeft_swap) ? node_swap_parent->lchild = state.node: node_swap_parent->rchild = state.node;
//     }
//     if (node_lchild && node_lchild != state.node_swap) node_lchild->parent = state.node_swap;
//     else if (node_parent && node_lchild == state.node_swap) node_parent = state.node_swap;
        
//     if (node_rchild && node_swap_rchild != state.node) node_rchild->parent = node_rchild;
//     else if (node_swap_rchild && node_swap_rchild == state.node) node_swap_rchild = state.node;

//     if (node_parent != state.node_swap && node_parent){
//         (state.oldIsLeft) ? node_parent->lchild = state.node_swap: node_parent->rchild = state.node_swap;
//     }
//     if (node_parent == nullptr){// node = root
//         HB_tree.root = state.node;
//     }
//     else if (node_swap_parent == nullptr){// node_swap = root
//         HB_tree.root = state.node_swap;
//     }
//     cout << "swap compelete!\n";
//     HB_tree.setPosition();
// }
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
    A->lchild = Bl;   if (Bl) Bl->parent = A;
    A->rchild = Br;   if (Br) Br->parent = A;

    B->lchild = Al;   if (Al) Al->parent = B;
    B->rchild = Ar;   if (Ar) Ar->parent = B;
    cout << "swap compelete!\n";
    HB_tree.setPosition();
}

inline Record M3_node_swapping(HBStarTree<ll>& HB_tree, vector<NodeBase<ll>*>& HB_node){
    // can only swap same kind of node 
    Record state{};
    // state.node = pick_node(HB_node); 
    do state.node = pick_node(HB_node);
    while (state.node->kind == NodeBase<ll>::Kind::Hierarchy);
    state.M = 3;
    state.oldParent = state.node->parent;
    if (state.oldParent){
        state.oldIsLeft = (state.oldParent->lchild == state.node);
        state.oldSibling = state.oldIsLeft ? state.oldParent->rchild : state.oldParent->lchild;
    }
    if (state.node->kind == NodeBase<ll>::Kind::Regular){
        // 只跟同樣為regular的人swap
        do state.node_swap = pick_node(HB_node);
        while (state.node_swap == state.node || state.node->kind == NodeBase<ll>::Kind::Hierarchy);
        state.oldParent_swap = state.node_swap->parent;
        if (state.oldParent_swap){
            state.oldIsLeft_swap = (state.oldParent_swap->lchild == state.node_swap);
            state.oldSibling_swap = state.oldIsLeft_swap ? state.oldParent_swap->rchild : state.oldParent_swap->lchild;
        }
        if (state.node_swap->kind == NodeBase<ll>::Kind::Regular){
            M3_regular_swap(state, HB_tree);
            cout << "swap finish!" << endl;
        }
        else {
            state.node = state.node_swap; // 改成內部swap
            state.oldParent = state.oldParent_swap;
            if (state.oldParent){
                state.oldIsLeft = state.oldIsLeft_swap;
                state.oldSibling = state.oldSibling_swap;
            }
        }
    }
    if (state.node->kind == NodeBase<ll>::Kind::Hierarchy){
        // swap 內部node
        // pick_blk(state, state.node->island->reps);
    }
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

        if (!state.is_self){
            swap(state.block2->ptr->width, state.block2->ptr->height);
            state.block2->ptr->rotate ^= 1;
        } 
        // 需set 內部的position!
        set_island_position(*state.node->island);
    }
    else if (state.node->kind == NodeBase<ll>::Kind::Regular){
        state.node->reg_node->rotate ^= 1;
        swap(state.node->reg_node->width, state.node->reg_node->height); // swap Node 的w,h
        swap(state.node->width, state.node->height); // swap NodeBase 的w,h 
    }
    HB_tree.setPosition();

    return state;
}

inline void M2_rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    if (state.node->kind == NodeBase<ll>::Kind::Hierarchy){
        swap(state.block1->ptr->width, state.block1->ptr->height); // swap Node 的w,h
        state.block1->ptr->rotate ^= 1;

        if (!state.is_self){
            swap(state.block2->ptr->width, state.block2->ptr->height);
            state.block2->ptr->rotate ^= 1;
        } 
        // 需set 內部的position!
        set_island_position(*state.node->island);
    }
    else if (state.node->kind == NodeBase<ll>::Kind::Regular){
        state.node->reg_node->rotate ^= 1;
        swap(state.node->reg_node->width, state.node->reg_node->height); // swap Node 的w,h
        swap(state.node->width, state.node->height); // swap NodeBase 的w,h 
    }
    HB_tree.setPosition();
}

inline void rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    switch(state.M){
        case 1: break;
        case 2: M2_rollback(state, HB_tree, HB_node);
            break;
        default: break;
    }
}

inline Record select_move(HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    return M3_node_swapping(HB_tree, HB_node);
}

inline void SA_algo(SA_Setting setting, Info& data, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
    setting.reset();
    double MT = 0, reject = 0, T = setting.T;
    int epoch = 0, uphill = 0, N = setting.k * data.hard_blocks.size();
    ll best_cost = area(data);
    ll prev_cost = best_cost;

    cout << "Initial best area: " << best_cost << "\n";
    // testing...
    Record state = select_move(HB_tree, HB_node);
    cout << "New cost: " << area(data) << endl;
    state = select_move(HB_tree, HB_node);
    cout << "New cost: " << area(data) << endl;
    state = select_move(HB_tree, HB_node);
    cout << "New cost: " << area(data) << endl;
    state = select_move(HB_tree, HB_node);
    cout << "New cost: " << area(data) << endl;
    
    /* store best sol. in x/y coor. */
    // vector<HardBlock> best_floorplan;

    // while((reject/(MT == 0 ? 1 : MT)) <= 0.95 && T >= setting.eps){
    //     MT = 0, reject = 0, uphill = 0;
    //     while(uphill <= N && MT <= 2*N){
    //         Record& state = select_move(HB_tree, HB_node);
    //         ll cur_cost = area(data);
    //         ll delta_c = cur_cost - prev_cost;
    //         uniform_real_distribution<double> dist(0.0, 1.0);
    //         MT++;
    //         if (delta_c <= 0){// accept
    //             prev_cost = cur_cost;
    //             if (cur_cost < best_cost){
    //                 best_floorplan.clear();
    //                 best_floorplan = data.hard_blocks;
    //                 best_cost = cur_cost;
    //             }
    //         }
    //         else{
    //             double random = dist(gen);
    //             if (random < exp(-delta_c/T)){// accept
    //                 uphill++;
    //                 prev_cost = cur_cost;
    //             }
    //             else{// reject
    //                 reject++;
    //                 rollback(state, HB_tree, HB_node);
    //             }
    //         }
    //     }
    //     T = T_secheduling(T, setting.r);
    //     cout << "Last cost:  " << prev_cost << " [ T: " << T << " ]\n";
    // }

    // data.hard_blocks = best_floorplan;
    // cout << "Final area: " << area(data) << "\n";
}