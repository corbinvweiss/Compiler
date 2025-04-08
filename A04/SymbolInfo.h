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
};

class IdentifierInfo : public SymbolInfo {
    private:
        Literal value;
    public:
        IdentifierInfo(Type t, Literal val);
        Literal getValue();
        TypeError typeError(Type rvalType); // check for a type mismatch.
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
        
};


