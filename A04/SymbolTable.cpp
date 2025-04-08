// SymbolTable.cpp
// Implement the SymbolTable ADT
// Corbin Weiss
// 2025-1-20

#include "SymbolTable.h"
#include <iostream>

IdentifierInfo::IdentifierInfo(std::string lexeme, Type t) 
    :lexeme(lexeme), type(t) {}

std::string IdentifierInfo::show() {
    return lexeme + " : " + typeToString(type) + " = " + LiteralToString(type, value);
}

std::string SymbolTable::get_name() {
    return this->name;
}

int SymbolTable::insert(IdentifierInfo* info) {
    // insert the key, value pair if the key is not already present
    // returns whether insertion was successful
    if(info == nullptr) {
        return 0;
    }
    std::string key = info->lexeme;
    if(this->symbols.find(key) == this->symbols.end()) {
        this->symbols[key] = info;
        return 1;
    }
    else {
        return 0;
    }
}

IdentifierInfo* SymbolTable::lookup(std::string key) {
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
        std::cout << "(" << it->first << ", " << it->second->show() << ") ";
    }
    std::cout << "\n";
}