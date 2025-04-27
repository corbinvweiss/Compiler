// SymbolTable.h
// Define the SymbolTable ADT
// Corbin Weiss
// 2025-1-20

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <string>
#include <vector>
#include "SymbolInfo.h"


class SymbolTable {
    private:
        std::string name;
        std::map<std::string, SymbolInfo*> symbols;
    public:
        // constructor 
        SymbolTable(std::string name) : name(name) {};
        SymbolTable() : name("") {};

        std::string get_name();
        /* 
            Insert a SymbolInfo object into the symbol table
            Returns 1 if successful, 0 if duplicate key.
        */
        int insert(std::string lexeme, SymbolInfo* info);
        /*
            Lookup the value associated with a key
            Returns value if found, -1 if not found
        */
        SymbolInfo* lookup(std::string key);
        /*
            Return the number of key, value pairs in the symbol table
        */
        int size();
        /*
            Displays the map in a human-readable format
        */
        void show();
        /*
            Return all SymbolInfo entries with the given type. Used for freeing arrays
        */
        std::vector<SymbolInfo*> FindLocalArrays();
};

#endif // SYMBOLTABLE_H