#include <iostream>
#include <string>
#include "DataTypes.hpp"

//Error check
void Info::print_blocks(){
    cout << "----PRINT BLOCKS START----\n";
    for (auto& blocks: hard_blocks){
        cout << "Block Name: " <<blocks.name << "  ";
        cout << "width: " << blocks.width << "  " << "height: " << blocks.height << "\n"; 
    }
    cout << "----PRINT BLOCKS END----\n";
}

void Info::print_symgroup(){
    cout << "----PRINT SYMGROUP START----\n";
    for (auto& symgroup: sym_groups){
        cout << "SymGroup Name: " <<symgroup.name << "\n";
        for (auto& pair: symgroup.pairs){
            cout << "Pair: " << pair.id_1 << "  "<<pair.id_2 << "\n";
        }
        for (auto& self: symgroup.selfs){
            cout << "selfs: " <<self.id << "\n";
        }
    }
    cout << "----PRINT SYMGROUP END----\n";
}