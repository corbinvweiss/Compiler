/*
SymbolInfo.h
Corbin Weiss
Created April 8, 2025

Define the abstract SymbolInfo data structure and its children FunctionInfo and IdentifierInfo
*/

#include <string>
#include <vector>

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
    none,
    i32,
    Bool,
    array_i32,
    array_bool,
};

using Literal = std::variant<int, bool>;

std::string typeToString(Type t);
std::string typeToString(std::vector<Type> types);
std::string LiteralToString(Type t, Literal l);

/*
Abstract base class defining structure of SymbolTable entry
*/
class SymbolInfo {
    private:
        Type return_type = Type::none;
    public:
        SymbolInfo(Type returnType);
        Type getReturnType();
        virtual std::string show() = 0; // display the symbol info in a human-readable format
};

class IdentifierInfo : public SymbolInfo {
    public:
        bool initialized;
        IdentifierInfo(Type t);
        std::string show();
};

class FunctionInfo : public SymbolInfo {
    private:
        std::vector<Type> param_list;
    public:
        FunctionInfo(Type returnType, std::vector<Type> paramTypes);
        // check for type mismatch between function signature and arguments
        // Return ArgNumber, ArgType, or None
        TypeError typeError(std::vector<Type> argTypes); 
        std::vector<Type> getParamList();
        std::string show();
        
};


