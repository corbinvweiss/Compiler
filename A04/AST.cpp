/*
AST.cpp
Corbin Weiss
27 March 2025

implement the Abstract Syntax Tree for Rustish
*/

#include "AST.h"

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

ASTNode::ASTNode() {}
ASTNode::~ASTNode() {}

ProgramNode::ProgramNode(ASTNode* main_def) 
    : main_def(main_def) {}

ProgramNode::~ProgramNode() {
    delete main_def;
}

MainDefNode::MainDefNode() {}
MainDefNode::~MainDefNode() {}
