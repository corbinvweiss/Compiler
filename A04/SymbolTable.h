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

using Literal = std::variant<int, bool>;

std::string typeToString(Type t);
std::string LiteralToString(Type t, Literal l);

// struct to hold information for the given symbol
// type, value
struct IdentifierInfo {
    std::string lexeme;
    Type type;
    Literal value = 0;
    IdentifierInfo(std::string lexeme, Type t);
    std::string show();
};

struct FunctionInfo {
    std::string lexeme;
    std::vector<Type> arglist;
    Type returnType;
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
    int insert(IdentifierInfo* info);
    /*
        Lookup the value associated with a key
        Returns value if found, -1 if not found
    */
    IdentifierInfo* lookup(std::string key);
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
    std::map<std::string, IdentifierInfo*> symbols;
};

#endif // SYMBOLTABLE_H