// #pragma once
// #include <string>
// #include <vector>
// #include <chrono>
// #include "Global_param.hpp"
// #include "HBStarTree.hpp"

// typedef int64_t ll;
// /**
//  * perturb 1: Node Movement
//  * perturb 2: Module Rotation
//  * perturb 3: Node Swapping
//  * perturb 4: Representative Change
//  * perturb 5: 
//  */
// class SA_Setting{
//     public:
//         SA_Setting() = default;
//         long long k = 5;
//         double eps = 1.0;
//         double r = 0.9;
//         double T = 1500.0;
//         void reset(){
//             k = 5; 
//             eps = 1.0;
//             r = 0.9;
//             T = 1500.0;
//         };
// };

// struct Record{
//     NodeBase<int64_t>*  node;  
//     // before-state 
//     NodeBase<int64_t>*  oldParent; // if null -> root
//     bool                oldIsLeft; 
//     NodeBase<int64_t>*  oldSibling; 
//     // after-state  
//     NodeBase<int64_t>*  newParent;
//     bool                newIsLeft;

//     int M = 1; // movement
// };

// ll area(Info& data){
//     ll maxX = 0, maxY = 0;
//     for (const auto& hb : data.hard_blocks) {
//         maxX = std::max(maxX, hb.ptr->x_abs + hb.width);
//         maxY = std::max(maxY, hb.ptr->y_abs + hb.height);
//     }
//     return maxX*maxY;
// }

// inline double T_secheduling(double T, double r){
//     return T * r;
// }
// // return 參數不確定這樣是什麼意思
// inline NodeBase<int64_t>* pick_node(vector<NodeBase<int64_t>*>& HB_node){
//     if (HB_node.empty()) return nullptr;
//     uniform_real_distribution<size_t> dist(0, HB_node.size() - 1);
//     return HB_node[dist(gen)];
// }
// // // 先不寫有root的情況
// // inline void M1_move_regular_node(Record& state, NodeBase<int64_t>* attached_node){
// //     bool is_root = (state.oldParent) ? false:true; // no parent = root
// //     bool is_empty_l = (attached_node->rchild) ? true:false; // 會傾向接右邊

// //     if (state.node->rchild && state.node->lchild){ // 2 child
// //         // 把左小孩接給父母+右小孩接另一邊最右(最左)
// //         (state.oldIsLeft) ? state.oldParent->lchild = state.node->lchild: state.oldParent->rchild = state.node->lchild;
// //         state.node->lchild->parent = state.oldParent;

// //         if (state.oldIsLeft){ // 右孩要接到最右邊
// //             NodeBase<int64_t>* r_most_node = state.oldParent->rchild;
// //             while(r_most_node->rchild) r_most_node = r_most_node->rchild;
// //             r_most_node->rchild = state.node->rchild;
// //             state.node->rchild->parent = r_most_node;
// //         }
// //         else{ // 右孩要接到最左邊
// //             NodeBase<int64_t>* l_most_node = state.oldParent->lchild;
// //             while(l_most_node->lchild) l_most_node = l_most_node->lchild;
// //             l_most_node->lchild = state.node->rchild;
// //             state.node->rchild->parent = l_most_node;
// //         }
// //     }   
// //     else if (state.node->rchild || state.node->lchild){ // 1 child
// //         bool is_r = (state.node->rchild) ? true:false; 
// //         // 把小孩接給父母
// //         if (state.oldIsLeft) {
// //             (is_r) ? state.oldParent->lchild = state.node->rchild :state.oldParent->lchild = state.node->lchild;
// //             (is_r) ? state.node->rchild->parent = state.oldParent :state.node->lchild->parent = state.oldParent;
// //         }
// //         else {
// //             (is_r) ? state.oldParent->rchild = state.node->rchild :state.oldParent->rchild = state.node->lchild;
// //             (is_r) ? state.node->rchild->parent = state.oldParent :state.node->lchild->parent = state.oldParent;
// //         }
// //     }
// //     else if (!state.node->rchild && !state.node->lchild){ // leaf
// //         (state.oldIsLeft) ? state.oldParent->lchild = nullptr: state.oldParent->rchild = nullptr; 
// //     }

// //     // 自己父母改掉
// //     (is_empty_l) ? attached_node->lchild = state.node: attached_node->rchild = state.node;
// //     state.node->parent = attached_node;
// //     // 建Record
// //     state.newParent = attached_node;
// //     state.newIsLeft = is_empty_l;
// // }

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
//     while(node == state.node || (node->rchild && node->lchild) || !node->parent) node = pick_node(HB_node); // 必有一邊無子
//     bool is_HB = (node->kind == NodeBase<ll>::Kind::Hierarchy) ? true:false;

//     if (state.node->kind == NodeBase<ll>::Kind::Regular){
//         //M1_move_regular_node(state, node);
//     }
//     // else if (state.node->kind == NodeBase<ll>::Kind::Hierarchy){

//     // }
//     // else{

//     // }
//     return state;
// }

// inline void rollback(Record state, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){

// }

// inline Record select_move(HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
//     return M1_move(HB_tree, HB_node);
// }

// inline void SA_algo(SA_Setting setting, Info& data, HBStarTree<int64_t>& HB_tree, vector<NodeBase<int64_t>*>& HB_node){
//     setting.reset();
//     double MT = 0, reject = 0, T = setting.T;
//     int epoch = 0, uphill = 0, N = setting.k * data.hard_blocks.size();
//     ll best_cost = area(data);
//     ll prev_cost = best_cost;

//     cout << "Initial best area: " << best_cost << "\n";
//     /* store best sol. in x/y coor. */
//     vector<HardBlock> best_floorplan;

//     while((reject/(MT == 0 ? 1 : MT)) <= 0.95 && T >= setting.eps){
//         MT = 0, reject = 0, uphill = 0;
//         while(uphill <= N && MT <= 2*N){
//             Record& state = select_move(HB_tree, HB_node);
//             ll cur_cost = area(data);
//             ll delta_c = cur_cost - prev_cost;
//             uniform_real_distribution<double> dist(0.0, 1.0);
//             MT++;
//             if (delta_c <= 0){// accept
//                 prev_cost = cur_cost;
//                 if (cur_cost < best_cost){
//                     best_floorplan.clear();
//                     best_floorplan = data.hard_blocks;
//                     best_cost = cur_cost;
//                 }
//             }
//             else{
//                 double random = dist(gen);
//                 if (random < exp(-delta_c/T)){// accept
//                     uphill++;
//                     prev_cost = cur_cost;
//                 }
//                 else{// reject
//                     reject++;
//                     rollback(state, HB_tree, HB_node);
//                 }
//             }
//         }
//         T = T_secheduling(T, setting.r);
//         cout << "Last cost:  " << prev_cost << " [ T: " << T << " ]\n";
//     }

//     data.hard_blocks = best_floorplan;
//     cout << "Final area: " << area(data) << "\n";
// }