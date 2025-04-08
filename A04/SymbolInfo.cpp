/*
SymbolInfo.cpp
Corbin Weiss
Created April 8, 2025
Compiler Construction

Implement the SymbolInfo, IdentifierInfo and FunctionInfo classes
*/

#include "SymbolInfo.h"

const char* typeNames[] = {
    "none",
    "i32",
    "bool",
    "[i32]",
    "[bool]"
};

std::string typeToString(Type t) {
    if (t >= Type::none && t <= Type::array_bool) {
        return typeNames[static_cast<int>(t)];
    }
    return "Unknown Type";
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

SymbolInfo::SymbolInfo(Type t) 
: return_type(t) {}
Type SymbolInfo::getReturnType() {
    return return_type;
}

IdentifierInfo::IdentifierInfo(Type t, Literal val)
: SymbolInfo(t), value(val) {}
Literal IdentifierInfo::getValue() {
    return value;
}

TypeError IdentifierInfo::typeError(Type rvalType) {
    if(getReturnType() != rvalType) {
        return TypeError::Assignment;
    }
    else {
        return TypeError::None;
    }
}

FunctionInfo::FunctionInfo(Type returnType, std::vector<Type> paramTypes) 
: SymbolInfo(returnType), param_list(paramTypes) {}

TypeError FunctionInfo::typeError(std::vector<Type> argTypes) {
    if(param_list.size() != argTypes.size()) {
        return TypeError::ArgNumber;
    }
    for(int i=0; i<param_list.size(); ++i) {
        if(param_list[i] != argTypes[i]) {
            return TypeError::ArgType;
        }
    }
    return TypeError::None;
}
std::vector<Type> FunctionInfo::getParamList() {
    return param_list;
}