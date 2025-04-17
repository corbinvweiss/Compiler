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

static FILE* FDOUT;   // file descriptor of a.s output file

int ERROR_COUNT;
void error(ErrorData err, std::string msg)
{
    std::cout << "error line " << err.line << ", column " << err.column << ": " + msg << "\n";
    std::cout << err.lineText;
    for(int i = 0; i < err.column - 1; i++)
        std::cout << "_";
    std::cout << "^\n";
}

void write(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(FDOUT, msg, args);
    va_end(args);
    fprintf(FDOUT, "\n");
}

void stalloc() {
    write("\taddi $sp, $sp, -4\t# allocate space on the stack. ");
}

void push(const char* reg) {
    stalloc();
    write("\tsw %s, 4($sp)   \t# load %s onto the stack.", reg, reg);
}

void pop(const char* reg) {
    write("\tlw %s, 4($sp)   \t# pop stack into %s", reg, reg);
    write("\taddi $sp, $sp, 4 \t# restore the stack");
}

ASTNode::ASTNode(ErrorData err) :err_data(err) {}

ProgramNode::ProgramNode(ASTNode* func_list, ASTNode* main, FILE* fdout) 
: ASTNode(ErrorData(nullptr, 0, 0))
{
    FDOUT = fdout;
    func_def_list = static_cast<FuncDefListNode*>(func_list);
    main_def = static_cast<MainDefNode*>(main);
    setGlobalST(new SymbolTable());
    func_def_list->TypeCheck();
    main_def->TypeCheck();
    EmitCode();
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

void ProgramNode::EmitCode() {
    std::cout << "Emitting code for ProgramNode\n";
    write("\t.data");
    write("\ttrue: .asciiz \"true\"\t# define the true string");
    write("\tfalse: .asciiz \"false\"\t# define the false string");
    write("\t.align 2");
    write("\t.text");
    write("\n\t### BEGIN ###");
    write("\tmove $fp, $sp\t\t# move the frame pointer to the top of the stack");
    write("\tjal __main\t# jump to the main function");
    write("\n\t### END ###");
    write("\tli $v0, 10\t#load value for exit");
    write("\tsyscall   \t#exit the program");

    main_def->EmitCode();
}

MainDefNode::MainDefNode(ASTNode* decl_list, ASTNode* stmts, ErrorData err) 
: ASTNode(err) 
{
        local_decl_list = static_cast<LocalDeclListNode*>(decl_list);
        stmt_list = static_cast<StatementListNode*>(stmts);
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
    std::cout << "---main---\n";
    LocalST->show();
    std::cout << "----------\n";
}

void MainDefNode::EmitCode() {
    std::cout << "Emitting code for MainDefNode\n";
    begin_func("main");
    stmt_list->EmitCode();
    end_func("main");
}


void begin_func(std::string name) {
    write("\n\t###########################");
    write("\t### \t %s \t ###", name.c_str());
    write("\t###########################");
    write("__%s:", name.c_str());
    push("$ra");
}

void end_func(std::string name) {
    write("\t### END OF FUNCTION \"%s\" ###", name.c_str());
    pop("$ra");
    write("\tjr $ra");
}


FuncDefNode::FuncDefNode(ASTNode* id, ASTNode* params, ASTNode* type, ASTNode* decl_list, ASTNode* stmts, ErrorData err) 
: ASTNode(err)
{
    identifier = static_cast<IdentifierNode*>(id);
    if(params) params_list = static_cast<ParamsListNode*>(params);
    else params_list = new ParamsListNode(err);    // empty parameter list
    if(type) return_type = static_cast<TypeNode*>(type);
    else return_type = new TypeNode(Type::none, err);
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
        error(err_data, "function '" + lexeme + "' already defined");
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
        error(err_data, "function '" + identifier->getLexeme() + 
        "' missing return of type '" + typeToString(getType()) + "'");
    }
    else {
        for(ASTNode* return_stmt: return_stmts) {
            if(return_stmt) {
                // expected void or some type, got something else
                if(getType().type != return_stmt->getType().type) {
                    error(return_stmt->err_data, "function '" + identifier->getLexeme() + "' expected return of type '"  + 
                    typeToString(getType()) + "' but got return of type '" + typeToString(return_stmt->getType()) + "'");
                }
            }
        }
    }
}

void FuncDefNode::EmitCode() {
    std::cout << "Emitting code for FuncDefNode\n";
}

ReturnNode::ReturnNode(ASTNode* expr, ErrorData err)
: ASTNode(err) 
{
    expression = expr;
}

ReturnNode::ReturnNode(ErrorData err)
: ASTNode(err) {}

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

void ReturnNode::EmitCode() {
    std::cout << "Emitting code for ReturnNode\n";
}

ParamsListNode::ParamsListNode(ASTNode* param, ErrorData err)
: ASTNode(err)
{
    parameters = new std::vector<VarDeclNode*> ();
    parameters->push_back(static_cast<VarDeclNode*>(param));
}
ParamsListNode::ParamsListNode(ErrorData err)
: ASTNode(err)
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

void ParamsListNode::EmitCode() {
    std::cout << "Emitting code for ParamsListNode\n";
}

FuncDefListNode::FuncDefListNode(ErrorData err) 
: ASTNode(err)
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

void FuncDefListNode::EmitCode() {
    std::cout << "Emitting code for FuncDefListNode\n";
}


LocalDeclListNode::LocalDeclListNode(ErrorData err) 
: ASTNode(err)
{
    decl_list = new std::vector<VarDeclNode*>();
}

LocalDeclListNode::LocalDeclListNode(ASTNode* decl, ErrorData err)
: ASTNode(err) 
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

void LocalDeclListNode::EmitCode() {
    std::cout << "Emitting code for LocalDeclListNode\n";
    for(VarDeclNode* decl : *decl_list) {
        decl->EmitCode();
    }
}

VarDeclNode::VarDeclNode(ASTNode* id, ASTNode* t, ErrorData err)
: ASTNode(err)
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
        // std::cout << lexeme << " : " << typeToString(getType()) << '\n';
    }
    else {
        error( err_data, "identifier '" + lexeme + "' redeclared");
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

void VarDeclNode::EmitCode() {
    std::cout << "Emitting code for VarDeclNode\n";
    stalloc();  // allocate space on the stack for a variable
}

ArrayDeclNode::ArrayDeclNode(ASTNode* id, ASTNode* tp, ASTNode* len, ErrorData err)
: ASTNode(err) {
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
        // std::cout << lexeme << " : " << typeToString(getType()) << '\n';
    }
    else {
        error(err_data, "identifier '" + lexeme + "' redeclared");
    }
}

void ArrayDeclNode::EmitCode() {
    std::cout << "Emitting code for ArrayDeclNode\n";
}

ArrayLiteralNode::ArrayLiteralNode(ASTNode* expression, ErrorData err)
: ASTNode(err)
{
    expressions = new std::vector<ASTNode*>();
    expressions->push_back(expression);
}

ArrayLiteralNode::~ArrayLiteralNode() {
    delete expressions;
}

void ArrayLiteralNode::append(ASTNode* expression) {
    expressions->push_back(expression);
} 

void ArrayLiteralNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
    for(ASTNode* expr : *expressions) {
        expr->setGlobalST(ST);
    }
}

void ArrayLiteralNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
    for(ASTNode* expr : *expressions) {
        expr->setLocalST(ST);
    }
}

void ArrayLiteralNode::TypeCheck() {
    for(ASTNode* expr : *expressions) {
        expr->TypeCheck();
    }
    if(!expressions->empty()) {
        TypeInfo firstType = expressions->front()->getType();
        if(!(firstType.type == Type::i32 || firstType.type == Type::Bool)) {
            error(err_data, "invalid array literal: expected bool or i32 but got type '" + typeToString(firstType) + "'");
            return;
        }
        int size = expressions->size();
        // check that all expressions have the same type as the first element
        for(ASTNode* expr : *expressions) { 
            if(expr->getType().type != firstType.type) {
                error(err_data, "invalid array literal: expected values of type '" + typeToString(firstType) 
                + "' but got '" + typeToString(expr->getType())+ "'");
                return;
            }
        }
        if(firstType.type == Type::i32) {
            setType(TypeInfo(Type::array_i32, size));
        }
        else if(firstType.type == Type::i32) {
            setType(TypeInfo(Type::array_bool, size));
        }
    }
}

void ArrayLiteralNode::EmitCode() {
    std::cout << "Emitting code for ArrayLiteralNode\n";
}


StatementListNode::StatementListNode(ErrorData err)
: ASTNode(err) 
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

void StatementListNode::EmitCode() {
    std::cout << "Emitting code for StatementListNode\n";
    for(ASTNode* stmt: *stmt_list) {
        stmt->EmitCode();
    }
}


AssignmentStatementNode::AssignmentStatementNode(ASTNode* id, ASTNode* expr, ErrorData err)
: ASTNode(err) 
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
        error(err_data, "cannot assign '" + typeToString(rtype) + "' to type '" + typeToString(ltype) + "'");
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

void AssignmentStatementNode::EmitCode() {
    // TODO: MIPS code to change value at identifier's offset from the frame pointer
    std::cout << "Emitting code for AssignmentStatementNode\n";
    expression->EmitCode(); // expression does its thing and stores its result at 4($sp)
    pop("$t0");             // fetch the expression's result into $t0
    std::string lexeme = identifier->getLexeme();
    SymbolInfo* info = LocalST->lookup(lexeme);
    assert(info);
    int offset = info->GetOffset();
    write("\tsw $t0, %d($fp)\t\t# write to '%s'", offset, lexeme.c_str());
}

ActualArgsNode::ActualArgsNode(ErrorData err) 
: ASTNode(err) 
{
    actual_args = new std::vector<ASTNode*>();
}

ActualArgsNode::ActualArgsNode(ASTNode* arg, ErrorData err)
: ASTNode(err) 
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

int ActualArgsNode::getSize() {
    return actual_args->size();
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

void ActualArgsNode::EmitCode() {
    std::cout << "Emitting code for ActualArgsNode\n";
    write("\t### Actual Args ###");
    // The arguments will be pulled off the stack in reverse order,
    // so we must put them on in reverse order
    for(auto arg = actual_args->rbegin(); arg != actual_args->rend(); ++arg) {
        (*arg)->EmitCode();
    }
    write("\t### END Actual Args");
}

CallNode::CallNode(ASTNode* id, ASTNode* act_args, ErrorData err) 
: ASTNode(err)
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
        error(err_data, "undefined function '" + lexeme + "'");
        return;
    }
    std::vector<TypeInfo> funcParams = funcInfo->getParamList();
    if(args.size() != funcParams.size()) {
        std::string msg = "wrong number of arguments: expected " + std::to_string(funcParams.size()) + " but got " + std::to_string(args.size())+ "";
        error(err_data, msg);
    }
    else {
        for(std::size_t i=0; i<funcParams.size(); ++i) {
            if(!(funcParams[i].type == args[i].type)) {
                error(err_data, "wrong type of arguments. Expected " + typeToString(funcParams) + 
                " but got " + typeToString(args) + "");
            }
        }
    }
}

void CallNode::EmitCode() {
    std::cout << "Emitting code for CallNode\n";
}

ArrayAccessNode::ArrayAccessNode(ASTNode* id, ASTNode* expr, ErrorData err) 
: LValueNode(err) 
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

std::string ArrayAccessNode::getLexeme() {
    return identifier->getLexeme();
}

void ArrayAccessNode::TypeCheck() {
    expression->TypeCheck();
    Type exprType = expression->getType().type;
    if(exprType != Type::i32) {
        error(err_data, "cannot use type '" + typeToString(exprType) + "' for array access");
    }
}

void ArrayAccessNode::initialize() {
    identifier->initialize();
}

void ArrayAccessNode::EmitCode() {
    std::cout << "Emitting code for ArrayAccessNode\n";
}

IfStatementNode::IfStatementNode(ASTNode* expr, ASTNode* if_, ASTNode* else_, ErrorData err) 
: ASTNode(err) 
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
        error(err_data, "expected boolean condition but got '" + typeToString(expression->getType()) + "'");
    }
    if_branch->TypeCheck();
    if (else_branch) {
        else_branch->TypeCheck();
    }
}

void IfStatementNode::EmitCode() {
    std::cout << "Emitting code for IfStatementNode\n";
}

WhileStatementNode::WhileStatementNode(ASTNode* expr, ASTNode* stmts, ErrorData err)
: ASTNode(err)
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
        error(err_data, "expected boolean condition but got '" + typeToString(expression->getType()) + "'");
    }
    body->TypeCheck();
}

void WhileStatementNode::EmitCode() {
    std::cout << "Emitting code for WhileStatementNode\n";
}

PrintStatementNode::PrintStatementNode(ASTNode* args, bool ln, ErrorData err)
: ASTNode(err)
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
            error(err_data, "cannot print type '" + typeToString(typeInfo) + "'");
        }
    }
}

void PrintStatementNode::EmitCode() {
    // get the arguments from the ActualArgsNode
    std::cout << "Emitting code for PrintStatementNode\n";
    write("\n\t### PrintStatement ###");
    actual_args->EmitCode();
    for(int i=0; i<actual_args->getSize(); i++) {
        pop("$a0");
        write("\tli $v0, 1     \t# print integer service");
        write("\tsyscall       \t# print the number");
        write("\tli $a0, 0x20  \t# load a space");
        write("\tli $v0, 11    \t# print character service");
        write("\tsyscall       \t# print the space");
    }
    if(newline) {
        write("\tli $a0, 0xA  \t# load a newline");
        write("\tli $v0, 11    \t# print character service");
        write("\tsyscall       \t# print the newline");
    }
}


UnaryNode::UnaryNode(std::string oper, ASTNode* r, ErrorData err)
: ASTNode(err) 
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
        error(err_data, "cannot apply operator " + op + 
            " to type " + typeToString(right->getType()));
    }
}

void UnaryNode::EmitCode() {
    std::cout << "Emitting code for UnaryNode\n";
}


BinaryNode::BinaryNode(std::string oper, ASTNode* l, ASTNode* r, ErrorData err)
: ASTNode(err)
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
        error(err_data, "cannot apply operator " + op + " to type " + typeToString(t));
        return false;
    }
}

void BinaryNode::TypeCheck() {
    left->TypeCheck();
    right->TypeCheck();
    TypeInfo lType = left->getType();
    TypeInfo rType = right->getType();
    // check that both operands are ints or bools
    BoolInt(lType);
    BoolInt(rType);
    // check that both operands are the same type
    if(!(lType.type == rType.type && lType.size == rType.size)) {
        error(err_data, "incompatible types '" + typeToString(lType) + "' and '" + typeToString(rType) + "'");
    }
    // check that both operands are compatible with the operator
    TypeInfo opType = GetOpType(op).op_type;
    if(lType.type != opType.type && opType.type != Type::any) {
        error(err_data, "ltype '" + typeToString(lType) + "' not compatible with operator '" + op + "'");
    }
    if(rType.type != opType.type && opType.type != Type::any) {
        error(err_data, "rtype '" + typeToString(rType) + "' not compatible with operator '" + op + "'");
    }

}

void BinaryNode::EmitCode() {
    std::cout << "Emitting code for BinaryNode\n";
}


IdentifierNode::IdentifierNode(std::string lexeme, ErrorData err)
    : LValueNode(err), lexeme(lexeme) {}

IdentifierNode::~IdentifierNode() {}

std::string IdentifierNode::getLexeme() {
    return lexeme;
}

void IdentifierNode::TypeCheck() {
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(info) { // identifier exists in local symbol table
        if(!info->IsInitialized()) {  // identifier is initialized
            error(err_data, "Identifier '" + lexeme + "' not initialized");
        }
    }
    else {
        error(err_data, "Identifier '" + lexeme + "' not found");
    }
}

TypeInfo IdentifierNode::getType() {
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(info) {
        return info->getReturnType();
    }
    else {
        error(err_data, "identifier '" + lexeme + "' not found");
    }
    return Type::none;
}

void IdentifierNode::initialize() {
    IdentifierInfo* info = static_cast<IdentifierInfo*>(LocalST->lookup(lexeme));
    if(info) info->Initialize();
}

void IdentifierNode::setGlobalST(SymbolTable* ST) {
    GlobalST = ST;
}

void IdentifierNode::setLocalST(SymbolTable* ST) {
    LocalST = ST;
}

void IdentifierNode::EmitCode() {
    std::cout << "Emitting code for IdentifierNode\n";
    SymbolInfo* info = LocalST->lookup(lexeme);
    assert(info);
    int offset = info->GetOffset();
    write("\tlw $t0, %d($fp)\t\t# get the value of '%s'", offset, lexeme.c_str());
    push("$t0");
}

TypeNode::TypeNode(TypeInfo t, ErrorData err) 
: ASTNode(err) 
{
    setType(t);
}
TypeNode::~TypeNode() {}

void TypeNode::EmitCode() {
    std::cout << "Emitting code for TypeNode\n";
}

NumberNode::NumberNode(int val, ErrorData err)
: ASTNode(err) 
{
    setType(Type::i32);
    value = val;
}
NumberNode::~NumberNode() {}
int NumberNode::getValue() {
    return value;
}

void NumberNode::EmitCode() {
    std::cout << "Emitting code for NumberNode\n";
    write("\tli, $t0, %d\t\t\t# load the value of the number", value);
    push("$t0");
}

BoolNode::BoolNode(bool val, ErrorData err)
: ASTNode(err) 
{
    setType(Type::Bool);
    value = val;
}
BoolNode::~BoolNode() {}
bool BoolNode::getValue() {
    return value;
}

void BoolNode::EmitCode() {
    std::cout << "Emitting code for BoolNode\n";
}