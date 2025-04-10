/*
SymbolInfo.h
Corbin Weiss
Created April 8, 2025

Define the abstract SymbolInfo data structure and its children FunctionInfo and IdentifierInfo
*/

#include <string>
#include <vector>
#define UNKNOWN_ARR INT_MAX

enum TypeError {
    None,
    Assignment,
    Undeclared,
    Uninitialized,
    ArgType,
    ArgNumber,
    MissingReturn,
    WrongReturn,
    RValue,
};

enum Type {
    i32,
    Bool,
    array_i32,
    array_bool,
    none,
    any
};


struct TypeInfo {
    Type type;
    int size;
    TypeInfo(Type t, int sz)
    : type(t), size(sz) {}
    TypeInfo(Type t)
    : type(t), size(0) {}
};

using Literal = std::variant<int, bool>;

std::string typeToString(TypeInfo t);
std::string typeToString(std::vector<TypeInfo> types);
std::string LiteralToString(TypeInfo t, Literal l);

/*
Abstract base class defining structure of SymbolTable entry
*/
class SymbolInfo {
    private:
        TypeInfo return_type = TypeInfo(Type::none);
    public:
        SymbolInfo(TypeInfo returnType);
        TypeInfo getReturnType();
        virtual std::string show() = 0; // display the symbol info in a human-readable format
};

class IdentifierInfo : public SymbolInfo {
    public:
        bool initialized;
        IdentifierInfo(TypeInfo t);
        std::string show();
};

class FunctionInfo : public SymbolInfo {
    private:
        std::vector<TypeInfo> param_list;
    public:
        FunctionInfo(TypeInfo returnType, std::vector<TypeInfo> paramTypes);
        // check for type mismatch between function signature and arguments
        // Return ArgNumber, ArgType, or None
        std::vector<TypeInfo> getParamList();
        std::string show();
        
};


