// SymbolTable.cpp
// Implement the SymbolTable ADT
// Corbin Weiss
// 2025-1-20

#include "SymbolTable.h"
#include <iostream>


std::string SymbolTable::get_name() {
    return this->name;
}

int SymbolTable::insert(std::string lexeme, SymbolInfo* info) {
    // insert the key, value pair if the key is not already present
    // returns whether insertion was successful
    if(info == nullptr) {
        return 0;
    }
    if(this->symbols.find(lexeme) == this->symbols.end()) {
        info->SetOffset(-4*(size())); // point to where the symbol is stored on the stack relative to $fp
        this->symbols[lexeme] = info;
        return 1;
    }
    else {
        return 0;
    }
}

SymbolInfo* SymbolTable::lookup(std::string key) {
    if (auto search = this->symbols.find(key); search != this->symbols.end()) {
        return search->second;
    }
    else {
        return nullptr;
    }
}

int SymbolTable::size() {
    return this->symbols.size();
}

void SymbolTable::show() {
    for(auto it = this->symbols.begin(); it != this->symbols.end(); ++it){
        std::cout << it->first << ": " << it->second->show() + "\n";
    }
}

std::vector<SymbolInfo*> SymbolTable::FindLocalArrays() {
    std::vector<SymbolInfo*> result = {};
    for(auto it = this->symbols.begin(); it != this->symbols.end(); ++it){
        Type t = it->second->getReturnType().type;
        if(t == Type::array_bool || t == Type::array_i32) {
            if(it->second->IsLocal()) {
                result.push_back(it->second);
            }
        }
    }
    return result;
}