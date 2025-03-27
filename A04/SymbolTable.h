// SymbolTable.h
// Define the SymbolTable ADT
// Corbin Weiss
// 2025-1-20

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <string>

enum Type {
    none,
    i32,
    Bool,
    array_i32,
    array_bool,
};

std::string typeToString(Type t);

// struct to hold information for the given symbol
// type, value
struct SymbolInfo {
    std::string lexeme;
    Type type;
    SymbolInfo(std::string lexeme, Type t)
        :lexeme(lexeme), type(t) {}
    std::string show();
};

class SymbolTable {
    // methods
public:
    // constructor 
    SymbolTable(std::string name) : name(name) {};
    SymbolTable() : name("") {};
    /*
        Return the name of the symbol table
    */
    std::string get_name();
    /* 
        Insert a SymbolInfo object into the symbol table
        Returns 1 if successful, 0 if duplicate key.
    */
    int insert(SymbolInfo* info);
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

private:
    std::string name;
    std::map<std::string, SymbolInfo*> symbols;
};

#endif // SYMBOLTABLE_H