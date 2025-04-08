/*
AST.cpp
Corbin Weiss
27 March 2025

implement the Abstract Syntax Tree for Rustish
*/

#include "AST.h"
#include <iostream>

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
    Type t = type->getType();
    IdentifierInfo* info = new IdentifierInfo(t);
    int valid = ST->insert(lexeme, info);
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

AssignmentStatementNode::AssignmentStatementNode(ASTNode* id, ASTNode* lit, int line)
: ASTNode(line) 
{
    identifier = static_cast<IdentifierNode*>(id);
    literal = static_cast<LiteralNode*>(lit);
}

AssignmentStatementNode::~AssignmentStatementNode() {
    delete identifier;
    delete literal;
}

void AssignmentStatementNode::UpdateSymbolTable(SymbolTable* ST) {
    // get the rvalue and rtype
    Literal rvalue = literal->getValue();
    Type rtype = literal->getType();
    // fetch the identifier's info
    std::string lexeme = identifier->get_lexeme();
    IdentifierInfo* info = static_cast<IdentifierInfo*>(ST->lookup(lexeme));
    TypeError err = info->setValue(rtype, rvalue);
    if(err == TypeError::Assignment) {
        std::cout << "error [line " << lineno << "]: Cannot assign '" 
            << typeToString(rtype) << "' to type '" << typeToString(info->getReturnType()) << "'.\n";
    }
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
    setType(t);
}
TypeNode::~TypeNode() {}

LiteralNode::LiteralNode(int val, int line)
: ASTNode(line) 
{
    setType(Type::i32);
    value = Literal(val);
}
LiteralNode::LiteralNode(bool val, int line)
: ASTNode(line)
{
    setType(Type::Bool);
    value = Literal(val);
}
LiteralNode::~LiteralNode() {}
Literal LiteralNode::getValue() {
    return value;
}