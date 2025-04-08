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

// **********************

SymbolInfo::SymbolInfo(Type t) 
: return_type(t) {}
Type SymbolInfo::getReturnType() {
    return return_type;
}

// **************************

IdentifierInfo::IdentifierInfo(Type t)
: SymbolInfo(t), initialized(false) {}

Literal IdentifierInfo::getValue() {
    return value;
}

TypeError IdentifierInfo::setValue(Type rtype, Literal rval) {
    if(getReturnType() != rtype) {
        return TypeError::Assignment;
    }
    else {
        value = rval;
        initialized = true;
        return TypeError::None;
    }
}

std::string IdentifierInfo::show() {
    std::string val = "";
    if(initialized) {
        val = LiteralToString(getReturnType(), value);
    }
    else {
        val = "undefined";
    }
    return typeToString(getReturnType()) + " = " + val;
}

// **************************

FunctionInfo::FunctionInfo(Type returnType, std::vector<Type> paramTypes) 
: SymbolInfo(returnType), param_list(paramTypes) {}

TypeError FunctionInfo::typeError(std::vector<Type> argTypes) {
    if(param_list.size() != argTypes.size()) {
        return TypeError::ArgNumber;
    }
    for(std::size_t i=0; i<param_list.size(); ++i) {
        if(param_list[i] != argTypes[i]) {
            return TypeError::ArgType;
        }
    }
    return TypeError::None;
}
std::vector<Type> FunctionInfo::getParamList() {
    return param_list;
}

std::string FunctionInfo::show() {
    std::string repr = "(";
    for(std::size_t i=0; i<param_list.size() - 1; ++i) {
        repr += typeToString(param_list[i]) + ", ";
    }
    repr += typeToString(param_list[param_list.size() - 1]) + ")";
    repr += " -> " + typeToString(getReturnType());
    return repr;
}

