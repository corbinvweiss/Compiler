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
    IdentifierInfo* info = new IdentifierInfo(lexeme, t);
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
    // std::cout << "in AssignmentStatmentNode::UpdateSymbolTable\n";
    // if(!number) {std::cout << "no number!\n";}
    Literal rvalue = literal->getValue();
    // std::cout << "getting rtype\n";
    Type rtype = literal->getType();
    std::string lexeme = identifier->get_lexeme();
    // std::cout << "looking up ltype\n";
    Type ltype = ST->lookup(lexeme)->type;
    // std::cout << "looking up info\n";
    // the types match, change the value of the identifier.
    if(ltype == rtype) {
        IdentifierInfo* info = ST->lookup(lexeme);
        info->value = rvalue;
        std::cout << "Updated value of " << lexeme << " to " 
            << LiteralToString(rtype, literal->getValue()) << ".\n";
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