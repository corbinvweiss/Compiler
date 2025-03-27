/*
AST.cpp
Corbin Weiss
27 March 2025

implement the Abstract Syntax Tree for Rustish
*/

#include "AST.h"
#include <iostream>

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

ProgramNode::ProgramNode(ASTNode* main) {
    symbol_table = new SymbolTable();
    main_def = static_cast<MainDefNode*>(main);
}

ProgramNode::~ProgramNode() {
    delete main_def;
}

MainDefNode::MainDefNode(ASTNode* decl_list) 
    {
        local_decl_list = static_cast<LocalDeclListNode*>(decl_list);
        symbol_table = new SymbolTable();
        local_decl_list->UpdateSymbolTable(symbol_table);
        symbol_table->show();
    }
MainDefNode::~MainDefNode() {
    delete local_decl_list;
}

LocalDeclListNode::LocalDeclListNode() {
    decl_list = new std::vector<VarDeclNode*>();
}
LocalDeclListNode::LocalDeclListNode(ASTNode* decl) {
    decl_list = new std::vector<VarDeclNode*>();
    decl_list->push_back(static_cast<VarDeclNode*>(decl));
}
void LocalDeclListNode::append(ASTNode* decl) {
    decl_list->push_back(static_cast<VarDeclNode*>(decl));
}
void LocalDeclListNode::UpdateSymbolTable(SymbolTable* ST) {
    for(VarDeclNode* declaration : *decl_list) {
        declaration->UpdateSymbolTable(ST);
    }
}
LocalDeclListNode::~LocalDeclListNode() {
    for (VarDeclNode *decl : *decl_list) {
        delete decl;
    }
}

VarDeclNode::VarDeclNode(ASTNode* id, ASTNode* t) {
    identifier = static_cast<IdentifierNode*>(id);
    type = static_cast<TypeNode*>(t);
}
VarDeclNode::~VarDeclNode() {
    delete identifier;
    delete type;
}
void VarDeclNode::UpdateSymbolTable(SymbolTable* ST) {
    std::string lexeme = identifier->get_lexeme();
    Type t = type->get_type();
    SymbolInfo* info = new SymbolInfo(lexeme, t);
    int valid = ST->insert(info);
    if(valid) {
        std::cout << lexeme << " : " << typeToString(t) << '\n';
    }
    else {
        std::cout << "Cannot redeclare identifier '" << lexeme << "'\n";
    }
}

IdentifierNode::IdentifierNode(std::string lexeme)
    : lexeme(lexeme) {}
IdentifierNode::~IdentifierNode() {}
std::string IdentifierNode::get_lexeme() {
    return lexeme;
}

TypeNode::TypeNode(Type t) {
    set_type(t);
}
TypeNode::~TypeNode() {}
