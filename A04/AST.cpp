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

int ERROR_COUNT;
void error(int line, std::string message) {
    std::cout << "error [line " << line << "]: " << message << "\n";
    ERROR_COUNT++;
}

ASTNode::ASTNode(int line) :lineno(line) {}

ProgramNode::ProgramNode(ASTNode* func_list, ASTNode* main) 
: ASTNode(0) 
{
    func_def_list = static_cast<FuncDefListNode*>(func_list);
    main_def = static_cast<MainDefNode*>(main);
    setGlobalST(new SymbolTable());
    func_def_list->TypeCheck();
    main_def->TypeCheck();
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

void MainDefNode::TypeCheck() {
    local_decl_list->TypeCheck();
    stmt_list->TypeCheck();
}

FuncDefNode::FuncDefNode(ASTNode* id, ASTNode* params, ASTNode* type, ASTNode* decl_list, ASTNode* stmts, int line) 
: ASTNode(line)
{
    identifier = static_cast<IdentifierNode*>(id);
    if(params) params_list = static_cast<ParamsListNode*>(params);
    else params_list = new ParamsListNode(line);    // empty parameter list
    if(type) return_type = static_cast<TypeNode*>(type);
    else return_type = new TypeNode(Type::none, line);
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

void FuncDefNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    local_decl_list->setGlobalST(ST);
    stmt_list->setGlobalST(ST);
}

void FuncDefNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    params_list->setLocalST(ST);    // Share local ST with parameters
    local_decl_list->setLocalST(ST);
    stmt_list->setLocalST(ST);
}


void FuncDefNode::TypeCheck() {
    std::string lexeme = identifier->getLexeme();
    FunctionInfo* info = new FunctionInfo(getType(), params_list->getTypes());
    int valid = GlobalST->insert(lexeme, info);
    if(!valid) {
        error(lineno, "function '" + lexeme + "' already defined.");
    }
    params_list->TypeCheck();   // put parameters in local ST
    local_decl_list->TypeCheck();
    stmt_list->TypeCheck();
    CheckReturn();
}

void FuncDefNode::CheckReturn() {
    std::vector<ASTNode*> return_stmts = stmt_list->FindReturns();
    // expected return type but got no return statements
    if(getType().type != Type::none && return_stmts.empty()) {
        error(lineno, "function '" + identifier->getLexeme() + 
        "' missing return of type '" + typeToString(getType()) + "'.");
    }
    else {
        for(ASTNode* return_stmt: return_stmts) {
            if(return_stmt) {
                // expected void or some type, got something else
                if(getType().type != return_stmt->getType().type) {
                    error(return_stmt->lineno, "function '" + identifier->getLexeme() + "' expected return of type '"  + 
                    typeToString(getType()) + "' but got return of type '" + typeToString(return_stmt->getType()) + "'.");
                }
            }
        }
    }
}

ReturnNode::ReturnNode(ASTNode* expr, int line)
: ASTNode(line) 
{
    expression = expr;
}

ReturnNode::ReturnNode(int line)
: ASTNode(line) {}

ReturnNode::~ReturnNode() {
    delete expression;
}

void ReturnNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    if(expression) expression->setGlobalST(ST);
}

void ReturnNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    if(expression) expression->setLocalST(ST);
}

void ReturnNode::TypeCheck() {
    if(expression) {
        setType(expression->getType());
    }
    else {
        setType(Type::none);
    }
}

ASTNode* ReturnNode::FindReturn() {
    return this;
}

ParamsListNode::ParamsListNode(ASTNode* param, int line)
: ASTNode(line)
{
    parameters = new std::vector<VarDeclNode*> ();
    parameters->push_back(static_cast<VarDeclNode*>(param));
}
ParamsListNode::ParamsListNode(int line)
: ASTNode(line)
{
    parameters = new std::vector<VarDeclNode*> ();
}

ParamsListNode::~ParamsListNode() {
    for(VarDeclNode* param : *parameters) {
        delete param;
    }
}

void ParamsListNode::append(ASTNode* param) {
    parameters->push_back(static_cast<VarDeclNode*>(param));
}

std::vector<TypeInfo> ParamsListNode::getTypes() {
    std::vector<TypeInfo> types = {};
    for(VarDeclNode* param: *parameters) {
        types.push_back(param->getType());
    }
    return types;
}

void ParamsListNode::setLocalST(SymbolTable* ST) {
    for(VarDeclNode* param : *parameters) {
        param->setLocalST(ST);
    }
}

void ParamsListNode::TypeCheck() {
    for(VarDeclNode* param: *parameters) {
        param->TypeCheck();
    }
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

void FuncDefListNode::TypeCheck() {
    for(FuncDefNode* func_def : *func_def_list) {
        func_def->TypeCheck();
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

void LocalDeclListNode::TypeCheck() {
    for(VarDeclNode* declaration : *decl_list) {
        declaration->TypeCheck();
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
void VarDeclNode::TypeCheck() {
    std::string lexeme = identifier->getLexeme();
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

ArrayDeclNode::ArrayDeclNode(ASTNode* id, ASTNode* tp, ASTNode* len, int line)
: ASTNode(line) {
    identifier = static_cast<IdentifierNode*>(id);
    type = static_cast<TypeNode*>(tp);
    length = static_cast<NumberNode*>(len);
    int size = length->getValue();
    if(type->getType().type == Type::i32) {
        setType(TypeInfo(Type::array_i32, size));
    }
    else if(type->getType().type == Type::Bool) {
        setType(TypeInfo(Type::array_bool, size));
    }
}

ArrayDeclNode::~ArrayDeclNode() {
    delete identifier;
    delete type;
    delete length;
}

void ArrayDeclNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
}

void ArrayDeclNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
}

void ArrayDeclNode::TypeCheck() {
    std::string lexeme = identifier->getLexeme();
    IdentifierInfo* info = new IdentifierInfo(getType());
    int valid = LocalST->insert(lexeme, info);
    if(valid) {
        std::cout << lexeme << " : " << typeToString(getType()) << '\n';
    }
    else {
        error(lineno, "identifier '" + lexeme + "' redeclared.");
    }
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

void StatementListNode::TypeCheck() {
    // std::cout << "in StatementListNode::UpdateSymbolTable\n";
    for(ASTNode* stmt : *stmt_list) {
        // std::cout << "iterating next statement\n";
        stmt->TypeCheck();
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

// find all return statements in a statement list
std::vector<ASTNode*> StatementListNode::FindReturns() {
    std::vector<ASTNode*> returns = {};
    for(ASTNode* stmt: *stmt_list) {
        ASTNode* return_stmt = stmt->FindReturn();
        if(return_stmt) {
            returns.push_back(return_stmt);
        }
    }
    return returns;
}


AssignmentStatementNode::AssignmentStatementNode(ASTNode* id, ASTNode* expr, int line)
: ASTNode(line) 
{
    identifier = static_cast<LValueNode*>(id);
    expression = expr;
}

AssignmentStatementNode::~AssignmentStatementNode() {
    delete identifier;
    delete expression;
}

void AssignmentStatementNode::TypeCheck() {
    expression->TypeCheck();
    // fetch the information for this identifier from the LocalST
    TypeInfo rtype = expression->getType();
    TypeInfo ltype = identifier->getType();
    if(!(ltype.type == rtype.type && ltype.size == rtype.size)) {
        std::cout << "error [line " << lineno << "]: Cannot assign '" 
            << typeToString(rtype) << "' to type '" << typeToString(ltype) << "'.\n";
    }
    else {
        identifier->initialize();
    }
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

ActualArgsNode::ActualArgsNode(int line) 
: ASTNode(line) 
{
    actual_args = new std::vector<ASTNode*>();
}

ActualArgsNode::ActualArgsNode(ASTNode* arg, int line)
: ASTNode(line) 
{
    actual_args = new std::vector<ASTNode*>();
    actual_args->push_back(arg);
}

ActualArgsNode::~ActualArgsNode() {
    for(ASTNode* arg : *actual_args) {
        delete arg;
    }
}

void ActualArgsNode::append(ASTNode* arg) {
    if(actual_args) actual_args->push_back(arg);
    else std::cout << "no actual_args\n";
}

void ActualArgsNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    for(ASTNode* arg : *actual_args) {
        arg->setGlobalST(ST);
    }
}

void ActualArgsNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    for(ASTNode* arg : *actual_args) {
        arg->setLocalST(ST);
    }
}

std::vector<TypeInfo> ActualArgsNode::argTypes() {
    std::vector<TypeInfo> types = {};
    for(ASTNode* arg : *actual_args) {
        types.push_back(arg->getType());
    }
    return types;
}

CallNode::CallNode(ASTNode* id, ASTNode* act_args, int line) 
: ASTNode(line)
{
    identifier = static_cast<IdentifierNode*>(id);
    actual_args = static_cast<ActualArgsNode*>(act_args);
}

CallNode::~CallNode() {
    delete identifier;
    delete actual_args;
}

void CallNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    identifier->setGlobalST(ST);
    actual_args->setGlobalST(ST);
}

void CallNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    identifier->setLocalST(ST);
    actual_args->setLocalST(ST);
}

TypeInfo CallNode::getType() {
    std::string lexeme = identifier->getLexeme();
    SymbolInfo* func = GlobalST->lookup(lexeme);
    return func->getReturnType();
}

void CallNode::TypeCheck() {
    std::vector<TypeInfo> args = actual_args->argTypes();
    std::string lexeme = identifier->getLexeme();
    FunctionInfo* funcInfo = static_cast<FunctionInfo*>(GlobalST->lookup(lexeme));
    if(!funcInfo) { // the function is not defined
        error(lineno, "undefined function '" + lexeme + "'.");
        return;
    }
    std::vector<TypeInfo> funcParams = funcInfo->getParamList();
    if(args.size() != funcParams.size()) {
        std::string msg = "wrong number of arguments: expected " + std::to_string(funcParams.size()) + " but got " + std::to_string(args.size())+ ".";
        error(lineno, msg);
    }
    else {
        for(std::size_t i=0; i<funcParams.size(); ++i) {
            if(!(funcParams[i].type == args[i].type)) {
                std::cout << "error [line " << lineno << "]: Wrong type of arguments. "
                << "Expected " << typeToString(funcParams) << " but got " << typeToString(args) << ".\n";
            }
        }
    }
}

ArrayAccessNode::ArrayAccessNode(ASTNode* id, ASTNode* expr, int line) 
: LValueNode(line) 
{
    identifier = static_cast<IdentifierNode*>(id);
    expression = expr;
}

ArrayAccessNode::~ArrayAccessNode() {
    delete identifier;
    delete expression;
}

void ArrayAccessNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    identifier->setGlobalST(ST);
    expression->setGlobalST(ST);
}

void ArrayAccessNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    identifier->setLocalST(ST);
    expression->setLocalST(ST);
}

TypeInfo ArrayAccessNode::getType() {
    // get the type of elements in the array
    std::string lexeme = identifier->getLexeme();
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(!info) return Type::none;
    TypeInfo tinfo = info->getReturnType();
    if(tinfo.type == Type::array_bool) return Type::Bool;
    if(tinfo.type == Type::array_i32) return Type::i32;
    else return Type::none;
}

void ArrayAccessNode::TypeCheck() {
    expression->TypeCheck();
    Type exprType = expression->getType().type;
    if(exprType != Type::i32) {
        error(lineno, "cannot use type '" + typeToString(exprType) + "' for array access.");
    }
}

void ArrayAccessNode::initialize() {
    identifier->initialize();
}

IfStatementNode::IfStatementNode(ASTNode* expr, ASTNode* if_, ASTNode* else_, int line) 
: ASTNode(line) 
{
    expression = expr;
    if_branch = static_cast<StatementListNode*>(if_);
    else_branch = static_cast<StatementListNode*>(else_);
}

IfStatementNode::~IfStatementNode() {
    delete expression;
    delete if_branch;
    if(else_branch) delete else_branch;
}

void IfStatementNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    expression->setGlobalST(ST);
    if_branch->setGlobalST(ST);
    if (else_branch) {
        else_branch->setGlobalST(ST);
    }
}

void IfStatementNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    expression->setLocalST(ST);
    if_branch->setLocalST(ST);
    if (else_branch) {
        else_branch->setLocalST(ST);
    }
}

void IfStatementNode::TypeCheck() {
    expression->TypeCheck();
    if(expression->getType().type != Type::Bool) {
        error(lineno, "expected boolean condition but got '" + typeToString(expression->getType()) + "'.");
    }
    if_branch->TypeCheck();
    if (else_branch) {
        else_branch->TypeCheck();
    }
}

WhileStatementNode::WhileStatementNode(ASTNode* expr, ASTNode* stmts, int line)
: ASTNode(line)
{
    expression = expr;
    body = static_cast<StatementListNode*>(stmts);
}

WhileStatementNode::~WhileStatementNode() {
    delete expression;
    delete body;
}

void WhileStatementNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    expression->setGlobalST(ST);
    body->setGlobalST(ST);
}

void WhileStatementNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    expression->setLocalST(ST);
    body->setLocalST(ST);
}

void WhileStatementNode::TypeCheck() {
    expression->TypeCheck();
    if(expression->getType().type != Type::Bool) {
        error(lineno, "expected boolean condition but got '" + typeToString(expression->getType()) + "'.");
    }
    body->TypeCheck();
}

PrintStatementNode::PrintStatementNode(ASTNode* args, bool ln, int line)
: ASTNode(line)
{
    newline = ln;
    actual_args = static_cast<ActualArgsNode*>(args);
}

PrintStatementNode::~PrintStatementNode() {
    delete actual_args;
}

void PrintStatementNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    actual_args->setGlobalST(ST);
}

void PrintStatementNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    actual_args->setLocalST(ST);
}

void PrintStatementNode::TypeCheck() {
    actual_args->TypeCheck();
    // make sure all arguments are bools or ints
    std::vector<TypeInfo> types = actual_args->argTypes();
    for(TypeInfo typeInfo : types) {
        if(!(typeInfo.type == Type::i32 || typeInfo.type == Type::Bool)) {
            error(lineno, "cannot print type '" + typeToString(typeInfo) + "'.");
        }
    }
}


UnaryNode::UnaryNode(std::string oper, ASTNode* r, int line)
: ASTNode(line) 
{
    op = oper;
    right = r;
    setType(GetOpType(op).return_type);
}

UnaryNode::~UnaryNode() {
    delete right;
}

void UnaryNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    right->setGlobalST(ST);
}

void UnaryNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    right->setLocalST(ST);
}

void UnaryNode::TypeCheck() {
    right->TypeCheck();
    TypeInfo opType = GetOpType(op).op_type;
    if(right->getType().type != opType.type) {
        error(lineno, "cannot apply operator " + op + 
            " to type " + typeToString(right->getType()));
    }
}


BinaryNode::BinaryNode(std::string oper, ASTNode* l, ASTNode* r, int line)
: ASTNode(line)
{
    op = oper;
    left = l;
    right = r;
    setType(GetOpType(op).return_type);
}

BinaryNode::~BinaryNode() {
    delete left;
    delete right;
}

void BinaryNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    left->setGlobalST(ST);
    right->setGlobalST(ST);
}

void BinaryNode::setLocalST(SymbolTable* ST) {
    GlobalST = ST;
    left->setLocalST(ST);
    right->setLocalST(ST);
}

bool BinaryNode::BoolInt(TypeInfo t) {
    if(t.type == Type::i32 || t.type == Type::Bool) {
        return true;
    }
    else {
        error(lineno, "cannot apply operator " + op + " to type " + typeToString(t));
        return false;
    }
}

void BinaryNode::TypeCheck() {
    left->TypeCheck();
    right->TypeCheck();
    TypeInfo lType = left->getType();
    TypeInfo rType = right->getType();
    // check that both operands are ints or bools
    if(!(BoolInt(lType) && BoolInt(rType))) {
        return;
    }
    // check that both operands are the same type
    if(!(lType.type == rType.type && lType.size == rType.size)) {
        error(lineno, "incompatible types '" + typeToString(lType) + "' and '" + typeToString(rType) + "'.");
        return;
    }
    // check that both operands are compatible with the operator
    TypeInfo opType = GetOpType(op).op_type;
    if(lType.type != opType.type && opType.type != Type::any) {
        error(lineno, "ltype '" + typeToString(lType) + "' not compatible with operator '" + op + "'.");
    }
    if(rType.type != opType.type && opType.type != Type::any) {
        error(lineno, "rtype '" + typeToString(rType) + "' not compatible with operator '" + op + "'.");
    }

}


IdentifierNode::IdentifierNode(std::string lexeme, int line)
    : LValueNode(line), lexeme(lexeme) {}

IdentifierNode::~IdentifierNode() {}

std::string IdentifierNode::getLexeme() {
    return lexeme;
}

void IdentifierNode::TypeCheck() {
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(info) { // identifier exists in local symbol table
        if(!info->initialized) {  // identifier is initialized
            std::cout << "error [line " << lineno << "]: " << "Identifier '" << lexeme << "' not initialized.\n";
        }
    }
    else {
        std::cout << "error [line " << lineno << "]: " << "Identifier '" << lexeme << "' not found.\n";
    }
}

TypeInfo IdentifierNode::getType() {
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(info) {
        return info->getReturnType();
    }
    else {
        error(lineno, "identifier '" + lexeme + "' not found.");
    }
    return Type::none;
}

void IdentifierNode::initialize() {
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(info) info->initialized = true;
}

void IdentifierNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
}

void IdentifierNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
}

TypeNode::TypeNode(TypeInfo t, int line) 
: ASTNode(line) 
{
    setType(t);
}
TypeNode::~TypeNode() {}

NumberNode::NumberNode(int val, int line)
: ASTNode(line) 
{
    setType(Type::i32);
    value = val;
}
NumberNode::~NumberNode() {}
int NumberNode::getValue() {
    return value;
}

BoolNode::BoolNode(bool val, int line)
: ASTNode(line) 
{
    setType(Type::Bool);
    value = val;
}
BoolNode::~BoolNode() {}
bool BoolNode::getValue() {
    return value;
}