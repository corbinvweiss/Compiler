/*
AST.cpp
Corbin Weiss
27 March 2025

implement the Abstract Syntax Tree for Rustish
*/

/*
Symbol table structure:
ProgramNode owns GlobalST, and shares it with the functions and main.
Each function owns its LocalST, and shares both the GlobalST and LocalST with its decls and statements
*/

#include "AST.h"
#include <iostream>

ASTNode::ASTNode(int line) :lineno(line) {}

ProgramNode::ProgramNode(ASTNode* func_list, ASTNode* main) 
: ASTNode(0) 
{
    func_def_list = static_cast<FuncDefListNode*>(func_list);
    main_def = static_cast<MainDefNode*>(main);
    setGlobalST(new SymbolTable());
}

ProgramNode::~ProgramNode() {
    delete func_def_list;
    delete main_def;
}

void ProgramNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    func_def_list->setGlobalST(ST);
    main_def->setGlobalST(ST);
}

void ProgramNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    func_def_list->setLocalST(ST);
    main_def->setLocalST(ST);
}

MainDefNode::MainDefNode(ASTNode* decl_list, ASTNode* stmts, int line) 
: ASTNode(line) 
{
        local_decl_list = static_cast<LocalDeclListNode*>(decl_list);
        stmt_list = static_cast<StatementListNode*>(stmts);
        std::cout << "Creating symbol table for main\n";
        setLocalST(new SymbolTable());

        local_decl_list->UpdateSymbolTable();
        LocalST->show();
        stmt_list->UpdateSymbolTable();
        LocalST->show();
}
MainDefNode::~MainDefNode() {
    delete local_decl_list;
}

void MainDefNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    local_decl_list->setGlobalST(ST);
    stmt_list->setGlobalST(ST);
}

void MainDefNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    local_decl_list->setLocalST(ST);
    stmt_list->setLocalST(ST);
}

FuncDefNode::FuncDefNode(ASTNode* id, ASTNode* params, ASTNode* type, ASTNode* decl_list, ASTNode* stmts, int line) 
: ASTNode(line)
{
    identifier = static_cast<IdentifierNode*>(id);
    params_list = static_cast<ParamsListNode*>(params);
    return_type = static_cast<TypeNode*>(type);
    local_decl_list = static_cast<LocalDeclListNode*>(decl_list);
    stmt_list = static_cast<StatementListNode*>(stmts);
    
    // share this function's symbol table with decl_list and stmt_list
    setLocalST(new SymbolTable());
    setType(return_type->getType());    // the type of the function is its return type.
}
FuncDefNode::~FuncDefNode() {
    delete identifier;
    delete params_list;
    delete return_type;
    delete local_decl_list;
    delete stmt_list;
}
void FuncDefNode::UpdateSymbolTable() {
    std::cout << "Adding function " << identifier->get_lexeme() << " to global symbol table\n";
    std::string lexeme = identifier->get_lexeme();
    FunctionInfo* info = new FunctionInfo(getType(), params_list->getTypes());
    GlobalST->insert(lexeme, info);
    GlobalST->show();
    std::cout << "Creating symbol table for function " << identifier->get_lexeme() << "\n";
    local_decl_list->UpdateSymbolTable();
    LocalST->show();
    stmt_list->UpdateSymbolTable();
    LocalST->show();
}

void FuncDefNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    local_decl_list->setGlobalST(ST);
    stmt_list->setGlobalST(ST);
}

void FuncDefNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    local_decl_list->setLocalST(ST);
    stmt_list->setLocalST(ST);
}

ParamsListNode::ParamsListNode(ASTNode* param, int line)
: ASTNode(line)
{
    parameters = new std::vector<VarDeclNode*> ();
    parameters->push_back(static_cast<VarDeclNode*>(param));
}

ParamsListNode::~ParamsListNode() {
    for(VarDeclNode* param : *parameters) {
        delete param;
    }
}

void ParamsListNode::append(ASTNode* param) {
    parameters->push_back(static_cast<VarDeclNode*>(param));
}

std::vector<Type> ParamsListNode::getTypes() {
    std::vector<Type> types = {};
    for(VarDeclNode* param: *parameters) {
        types.push_back(param->getType());
    }
    return types;
}

FuncDefListNode::FuncDefListNode(int line) 
: ASTNode(line)
{
    func_def_list = new std::vector<FuncDefNode*> ();
}

FuncDefListNode::~FuncDefListNode() {
    for(FuncDefNode* func_def : *func_def_list) {
        delete func_def;
    }
}

void FuncDefListNode::append(ASTNode* func_def) {
    func_def_list->push_back(static_cast<FuncDefNode*>(func_def));
}

void FuncDefListNode::UpdateSymbolTable() {
    for(FuncDefNode* func_def : *func_def_list) {
        func_def->UpdateSymbolTable();
    }
}

void FuncDefListNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    for(FuncDefNode* func_def : *func_def_list) {
        func_def->setGlobalST(ST);
    }
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

LocalDeclListNode::~LocalDeclListNode() {
    for (VarDeclNode *decl : *decl_list) {
        delete decl;
    }
}

void LocalDeclListNode::append(ASTNode* decl) {
    decl_list->push_back(static_cast<VarDeclNode*>(decl));
}

void LocalDeclListNode::UpdateSymbolTable() {
    for(VarDeclNode* declaration : *decl_list) {
        declaration->UpdateSymbolTable();
    }
}

void LocalDeclListNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    for(VarDeclNode* declaration : *decl_list) {
        declaration->setGlobalST(ST);
    }
}

void LocalDeclListNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    for(VarDeclNode* declaration : *decl_list) {
        declaration->setLocalST(ST);
    }
}

VarDeclNode::VarDeclNode(ASTNode* id, ASTNode* t, int line)
: ASTNode(line)
{
    identifier = static_cast<IdentifierNode*>(id);
    type = static_cast<TypeNode*>(t);
    setType(type->getType());
}
VarDeclNode::~VarDeclNode() {
    delete identifier;
    delete type;
}
void VarDeclNode::UpdateSymbolTable() {
    std::string lexeme = identifier->get_lexeme();
    IdentifierInfo* info = new IdentifierInfo(getType());
    int valid = LocalST->insert(lexeme, info);
    if(valid) {
        std::cout << lexeme << " : " << typeToString(getType()) << '\n';
    }
    else {
        std::cout << "error [line " << lineno << "]: identifier '" << lexeme << "' redeclared.\n";
    }
}

void VarDeclNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    identifier->setGlobalST(ST);
}

void VarDeclNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    identifier->setLocalST(ST);
}

StatementListNode::StatementListNode(int line)
: ASTNode(line) 
{
    stmt_list = new std::vector<ASTNode*>();
}

StatementListNode::~StatementListNode() {
    for (ASTNode* stmt : *stmt_list) {
        delete stmt;
    }
    delete stmt_list;
}

void StatementListNode::append(ASTNode* stmt) {
    stmt_list->push_back(stmt);
}

void StatementListNode::UpdateSymbolTable() {
    // std::cout << "in StatementListNode::UpdateSymbolTable\n";
    for(ASTNode* stmt : *stmt_list) {
        // std::cout << "iterating next statement\n";
        stmt->UpdateSymbolTable();
    }
}

void StatementListNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    for(ASTNode* stmt : *stmt_list) {
        stmt->setGlobalST(ST);
    }
}

void StatementListNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    for(ASTNode* stmt : *stmt_list) {
        stmt->setLocalST(ST);
    }
}


AssignmentStatementNode::AssignmentStatementNode(ASTNode* id, ASTNode* expr, int line)
: ASTNode(line) 
{
    identifier = static_cast<IdentifierNode*>(id);
    expression = static_cast<ExpressionNode*>(expr);
}

AssignmentStatementNode::~AssignmentStatementNode() {
    delete identifier;
    delete expression;
}

void AssignmentStatementNode::UpdateSymbolTable() {

    identifier->setValue(expression);
    return;
}

void AssignmentStatementNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    identifier->setGlobalST(ST);
    expression->setGlobalST(ST);
}

void AssignmentStatementNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    identifier->setLocalST(ST);
    expression->setLocalST(ST);
}

ExpressionNode::ExpressionNode(int line)
: ASTNode(line) {}
ExpressionNode::~ExpressionNode() {}

IdentifierNode::IdentifierNode(std::string lexeme, int line)
    : ExpressionNode(line), lexeme(lexeme) {}

IdentifierNode::~IdentifierNode() {}

std::string IdentifierNode::get_lexeme() {
    return lexeme;
}

Literal IdentifierNode::getValue() {
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(info) { // identifier exists in local symbol table
        if(info->isInitialized()) {
            return info->getValue(); 
        }
        else {
            std::cout << "error [line " << lineno << "]: " << "Identifier '" << lexeme << "' not initialized.\n";
        }
        
    }
    else {
        std::cout << "error [line " << lineno << "]: " << "Identifier '" << lexeme << "' not found.\n";
    }
    return {0}; // todo: return a nullptr instead of this janky thing
}

void IdentifierNode::setValue(ExpressionNode* expr) {
    Type rtype = expr->getType();
    Literal rval = expr->getValue();
    // fetch the existing information for this identifier from the LocalST
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(!info) { // identifier not found because it has not been declared.
        std::cout << "error [line " << lineno << "]: Identifier '" << lexeme << "'" << " not declared.\n";
        return;
    }
    TypeError err = info->setValue(rtype, rval);
    if(err == TypeError::Assignment) {
        std::cout << "error [line " << lineno << "]: Cannot assign '" 
            << typeToString(rtype) << "' to type '" << typeToString(info->getReturnType()) << "'.\n";
    }
}

void IdentifierNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
}

void IdentifierNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
}

TypeNode::TypeNode(Type t, int line) 
: ASTNode(line) 
{
    setType(t);
}
TypeNode::~TypeNode() {}

LiteralNode::LiteralNode(int val, int line)
: ExpressionNode(line) 
{
    setType(Type::i32);
    value = Literal(val);
}
LiteralNode::LiteralNode(bool val, int line)
: ExpressionNode(line)
{
    setType(Type::Bool);
    value = Literal(val);
}
LiteralNode::~LiteralNode() {}
Literal LiteralNode::getValue() {
    return value;
}