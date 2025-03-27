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

MainDefNode::MainDefNode(ASTNode* func_body) 
    :func_body(func_body) {}
MainDefNode::~MainDefNode() {
    delete func_body;
}

FuncBodyNode::FuncBodyNode(ASTNode* local_decl_list)
    : local_decl_list(local_decl_list) {}
FuncBodyNode::~FuncBodyNode() {
    delete local_decl_list;
}

LocalDeclListNode::LocalDeclListNode() {
    decl_list = new std::vector<ASTNode*>();
}
LocalDeclListNode::LocalDeclListNode(ASTNode* decl) {
    decl_list = new std::vector<ASTNode*>();
    decl_list->push_back(decl);
}
void LocalDeclListNode::append(ASTNode* decl) {
    decl_list->push_back(decl);
}
LocalDeclListNode::~LocalDeclListNode() {
    for (ASTNode *decl : *decl_list) {
        delete decl;
    }
}

VarDeclNode::VarDeclNode(ASTNode* id, ASTNode* t) 
    : identifier(id), type(t) {
        // here is where I should put the identifier and its type in the symbol table
    }
VarDeclNode::~VarDeclNode() {
    delete identifier;
    delete type;
}

IdentifierNode::IdentifierNode(std::string lexeme)
    : lexeme(lexeme) {}
IdentifierNode::~IdentifierNode() {}

TypeNode::TypeNode(Type t) {
    set_type(t);
}
TypeNode::~TypeNode() {}
