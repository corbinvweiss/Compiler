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

ProgramNode::ProgramNode(ASTNode* main) 
: ASTNode(0) 
{
    symbol_table = new SymbolTable();
    main_def = static_cast<MainDefNode*>(main);
}

ProgramNode::~ProgramNode() {
    delete main_def;
}

MainDefNode::MainDefNode(ASTNode* decl_list, ASTNode* stmts, int line) 
: ASTNode(line) 
{
        local_decl_list = static_cast<LocalDeclListNode*>(decl_list);
        stmt_list = static_cast<StatementListNode*>(stmts);
        symbol_table = new SymbolTable();
        local_decl_list->UpdateSymbolTable(symbol_table);
        symbol_table->show();
        stmt_list->UpdateSymbolTable(symbol_table);
        symbol_table->show();
}
MainDefNode::~MainDefNode() {
    delete local_decl_list;
}

LocalDeclListNode::LocalDeclListNode(int line) 
: ASTNode(line)
{
    decl_list = new std::vector<VarDeclNode*>();
}
LocalDeclListNode::LocalDeclListNode(ASTNode* decl, int line)
: ASTNode(line) 
{
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

VarDeclNode::VarDeclNode(ASTNode* id, ASTNode* t, int line)
: ASTNode(line)
{
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
        std::cout << "error [line " << lineno << "]: identifier '" << lexeme << "' redeclared.\n";
    }
}

StatementListNode::StatementListNode(int line)
: ASTNode(line) 
{
    stmt_list = new std::vector<ASTNode*>();
}

void StatementListNode::append(ASTNode* stmt) {
    stmt_list->push_back(stmt);
}

void StatementListNode::UpdateSymbolTable(SymbolTable* ST) {
    // std::cout << "in StatementListNode::UpdateSymbolTable\n";
    for(ASTNode* stmt : *stmt_list) {
        // std::cout << "iterating next statement\n";
        stmt->UpdateSymbolTable(ST);
    }
}

StatementListNode::~StatementListNode() {
    for (ASTNode* stmt : *stmt_list) {
        delete stmt;
    }
    delete stmt_list;
}

AssignmentStatementNode::AssignmentStatementNode(ASTNode* id, ASTNode* literal, int line)
: ASTNode(line) 
{
    identifier = static_cast<IdentifierNode*>(id);
    number = static_cast<NumberNode*>(literal);
}

AssignmentStatementNode::~AssignmentStatementNode() {
    delete identifier;
    delete number;
}

void AssignmentStatementNode::UpdateSymbolTable(SymbolTable* ST) {
    // std::cout << "in AssignmentStatmentNode::UpdateSymbolTable\n";
    // if(!number) {std::cout << "no number!\n";}
    Literal rvalue = {number->get_value()};
    // std::cout << "getting rtype\n";
    Type rtype = number->get_type();
    std::string lexeme = identifier->get_lexeme();
    // std::cout << "looking up ltype\n";
    Type ltype = ST->lookup(lexeme)->type;
    // std::cout << "looking up info\n";
    // the types match, change the value of the identifier.
    if(ltype == rtype) {
        SymbolInfo* info = ST->lookup(lexeme);
        info->value = rvalue;
        std::cout << "Updated value of " << lexeme << " to " 
            << number->get_value() << ".\n";
    }
    else {
        std::cout << "error [line " << lineno << "]: Cannot assign '" 
            << typeToString(rtype) << "' to type '" << typeToString(ltype) << "'.\n";
    }
    // check if the rtype matches the ltype
    // std::cout << "Updating the symbol table for assignmentStatement\n";
    return;
}

IdentifierNode::IdentifierNode(std::string lexeme, int line)
    : ASTNode(line), lexeme(lexeme) {}
IdentifierNode::~IdentifierNode() {}
std::string IdentifierNode::get_lexeme() {
    return lexeme;
}

TypeNode::TypeNode(Type t, int line) 
: ASTNode(line) 
{
    set_type(t);
}
TypeNode::~TypeNode() {}

// todo: convert to LiteralNode
NumberNode::NumberNode(int value, int line)
: ASTNode(line), value(value) {
    set_type(Type::i32);
}

NumberNode::~NumberNode() {}

int NumberNode::get_value() {
    return value;
}