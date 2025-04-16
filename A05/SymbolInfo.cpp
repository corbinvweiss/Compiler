/*
SymbolInfo.cpp
Corbin Weiss
Created April 8, 2025
Compiler Construction

Implement the SymbolInfo, IdentifierInfo and FunctionInfo classes
*/

#include "SymbolInfo.h"

const char* typeNames[] = {
    "i32",
    "bool",
    "[i32",
    "[bool",
    "none",
    "any"
};

std::string typeToString(TypeInfo t) {
    if (t.type >= Type::i32 && t.type <= Type::Bool) {
        return typeNames[static_cast<int>(t.type)];
    }
    else if(t.type >= Type::array_i32 && t.type <= Type::array_bool) {
        std::string repr = typeNames[static_cast<int>(t.type)];
        if(t.size < UNKNOWN_ARR) {  // sized array
            repr += "; " + std::to_string(t.size) + "]";
        }
        else {  // unsized array
            repr += "]";
        }
        return repr;
    }
    return "none";
}

std::string typeToString(std::vector<TypeInfo> types) {
    std::string result = "(";
    for(std::size_t i=0; i < types.size() - 1; ++i) {
        result += typeToString(types[i]) + ", ";
    }
    result += typeToString(types[types.size() - 1]) + ")";
    return result;
}

std::string LiteralToString(Type type, Literal value) {
    std::string val = "";
    if(type==Type::i32 && std::holds_alternative<int>(value)) {
        val = std::to_string(std::get<int>(value));
    }
    else if(type==Type::Bool && std::holds_alternative<bool>(value)) {
        val = std::to_string(std::get<bool>(value));
    }
    return val;
}

// **********************

SymbolInfo::SymbolInfo(TypeInfo t) 
: return_type(t) {}
TypeInfo SymbolInfo::getReturnType() {
    return return_type;
}

// **************************

std::string IdentifierInfo::show() {
    std::string init = "undefined";
    if(initialized) {
        init = "initialized";
    }
    return typeToString(getReturnType()) + " : " + std::to_string(GetOffset());
}

// **************************

FunctionInfo::FunctionInfo(TypeInfo returnType, std::vector<TypeInfo> paramTypes) 
: SymbolInfo(returnType), param_list(paramTypes) {}


std::vector<TypeInfo> FunctionInfo::getParamList() {
    return param_list;
}

std::string FunctionInfo::show() {
    std::string repr = "(";
    if(param_list.size()) {
        for(std::size_t i=0; i<param_list.size() - 1; ++i) {
            repr += typeToString(param_list[i]) + ", ";
        }
        repr += typeToString(param_list[param_list.size() - 1]);
    } 
    repr += ")";
    repr += " -> " + typeToString(getReturnType());
    return repr;
}

