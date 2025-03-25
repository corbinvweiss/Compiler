/*
parsetree.cpp
Corbin Weiss
2025-3-12

implement a parse tree that can print itself as outlined in 
https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/parser.html
*/

#include <iostream>
#include "parsetree.h"

// Convert Operator enum to string
std::string operatorToString(Operator op) {
    switch (op) {
        case Operator::_PLUS: return "+";
        case Operator::_MINUS: return "-";
        case Operator::_TIMES: return "*";
        case Operator::_DIVIDE: return "/";
        case Operator::_MODULUS: return "%";
        case Operator::_AND: return "&&";
        case Operator::_OR: return "||";
        case Operator::_EQ: return "==";
        case Operator::_NE: return "!=";
        case Operator::_LT: return "<";
        case Operator::_LE: return "<=";
        case Operator::_GT: return ">";
        case Operator::_GE: return ">=";
        case Operator::_NOT: return "!";
        default: return "Unknown Operator";
    }
}


// print spaces to indent nodes in the tree
void tab(int depth) {
    for(int i=0; i<depth; i++) {
        std::cout << "    ";
    }
}

const char* typeNames[] = {
    "none",
    "i32",
    "bool",
    "[i32]",
    "[bool]"
};

std::string typeToString(Type t) {
    if (t >= Type::NONE && t <= Type::ARRAY_BOOL) {
        return typeNames[static_cast<int>(t)];
    }
    return "Unknown Type";
}

ParseTreeNode::ParseTreeNode() {}
ParseTreeNode::~ParseTreeNode() {}

ProgramNode::ProgramNode(ParseTreeNode* func_def_list, ParseTreeNode* main_def):
    func_def_list(func_def_list), main_def(main_def) {}
ProgramNode::~ProgramNode() {
    delete func_def_list;
    delete main_def;
}
void ProgramNode::show(int depth) {
    tab(depth);
    std::cout << "program\n";
    func_def_list->show(depth+1);
    main_def->show(depth+1);
}

FuncDefListNode::FuncDefListNode() {
    func_def_list = new std::vector<ParseTreeNode*> ();
}
FuncDefListNode::~FuncDefListNode() {
    for (ParseTreeNode* func : *func_def_list) {
        delete func;
    }
}
void FuncDefListNode::append(ParseTreeNode *func_def) {
    // add the func_def to the vector of pointers to func_defs
    func_def_list->push_back(func_def);
}

void FuncDefListNode::show(int depth) {
    tab(depth);
    std::cout << "func_def_list\n";
    for (ParseTreeNode* func: *func_def_list) {
        func->show(depth+1);
    }
}

// constructor for function definition with parameters and return type
FuncDefNode::FuncDefNode(ParseTreeNode *identifier, 
    ParseTreeNode *parameters, ParseTreeNode *returntype, ParseTreeNode *body)
    :identifier(identifier), parameters(parameters), returntype(returntype), body(body) {}
// constructor for function definition with no return type
FuncDefNode::FuncDefNode(ParseTreeNode *identifier, 
    ParseTreeNode *parameters, ParseTreeNode *body)
    :identifier(identifier), parameters(parameters), 
    returntype(new TypeNode(Type::NONE)), body(body) {}
// constructor for function definition with no parameters or return type; 
FuncDefNode::FuncDefNode(ParseTreeNode *identifier, ParseTreeNode *body) 
    :identifier(identifier), parameters(new ParamsListNode()), 
    returntype(new TypeNode(Type::NONE)), body(body) {}
FuncDefNode::~FuncDefNode() {
    delete identifier;
    delete body;
}
void FuncDefNode::show(int depth) {
    tab(depth);
    std::cout << "func_def\n";
    identifier->show(depth+1);
    parameters->show(depth+1);
    returntype->show(depth+1);
    body->show(depth+1);
}

MainDefNode::MainDefNode(ParseTreeNode *body):
    body(body) {}

MainDefNode::~MainDefNode() {
    delete body;
}

void MainDefNode::show(int depth) {
    tab(depth);
    std::cout << "main_def\n";
    if(body) {
        body->show(depth+1);
    }
}

// initializing parameter list with a parameter
ParamsListNode::ParamsListNode(ParseTreeNode* param) {
    params_list = new std::vector<ParseTreeNode*> ();
    params_list->push_back(param);
}
// initializing an empty parameter list
ParamsListNode::ParamsListNode() {
    params_list = new std::vector<ParseTreeNode*> ();
}
ParamsListNode::~ParamsListNode() {
    for (ParseTreeNode* param : *params_list) {
        delete param;
    }
}
void ParamsListNode::append(ParseTreeNode *param) {
    // add the parameter to the vector of pointers to parameters
    params_list->push_back(param);
}
void ParamsListNode::show(int depth) {
    tab(depth);
    std::cout << "param_list\n";
    for (ParseTreeNode* param: *params_list) {
        param->show(depth+1);
    }
}

VarDeclNode::VarDeclNode(ParseTreeNode *identifier, ParseTreeNode *type)
    :identifier(identifier), type(type) {}
VarDeclNode::~VarDeclNode() {
    delete identifier;
    delete type;
}
void VarDeclNode::show(int depth) {
    tab(depth);
    std::cout << "var_decl\n";
    identifier->show(depth+1);
    type->show(depth+1);
}

TypeNode::TypeNode(Type type)
    :type(type) {}
TypeNode::~TypeNode() {}
void TypeNode::show(int depth) {
    tab(depth);
    std::string str_type = "";
    std::cout << "type: " << typeToString(type) << "\n";
}

FuncBodyNode::FuncBodyNode(ParseTreeNode *local_decl_list, ParseTreeNode *statement_list) 
    :local_decl_list(local_decl_list), statement_list(statement_list) {}
FuncBodyNode::~FuncBodyNode() {
    delete local_decl_list;
    delete statement_list;
}
void FuncBodyNode::show(int depth) {
    tab(depth);
    std::cout << "func_body\n";
    local_decl_list->show(depth+1);
    statement_list->show(depth+1);
}

LocalDeclListNode::LocalDeclListNode() {
    local_decl_list = new std::vector<ParseTreeNode *> ();
}
LocalDeclListNode::~LocalDeclListNode() {
    for (ParseTreeNode *declaration : *local_decl_list) {
        delete declaration;
    }
}
void LocalDeclListNode::append(ParseTreeNode *declaration) {
    local_decl_list->push_back(declaration);
}
void LocalDeclListNode::show(int depth) {
    tab(depth);
    std::cout << "local_decl_list\n";
    if(!local_decl_list->empty()) {
        for (ParseTreeNode *declaration : *local_decl_list) {
            declaration->show(depth+1);
        }
    }
    else {
        tab(depth+1);
        std::cout << "(none)\n";
    }
}

StatementListNode::StatementListNode() {
    statement_list = new std::vector<ParseTreeNode *> ();
}
StatementListNode::StatementListNode(ParseTreeNode *statement) {
    statement_list = new std::vector<ParseTreeNode *> ();
    statement_list->push_back(statement);
}
StatementListNode::~StatementListNode() {
    for(auto statement : *statement_list) {
        delete statement;
    }
}
void StatementListNode::append(ParseTreeNode *statement) {
    statement_list->push_back(statement);
}
void StatementListNode::show(int depth) {
    tab(depth);
    std::cout << "statement_list\n";
    if(!statement_list->empty()) {
        for (ParseTreeNode *statement : *statement_list) {
            statement->show(depth+1);
        }
    }
    else {
        tab(depth+1);
        std::cout << "(none)\n";
    }
}

StatementNode::StatementNode() {}
StatementNode::StatementNode(ParseTreeNode *contents)
    :contents(contents) {}
StatementNode::~StatementNode() {}
void StatementNode::show(int depth) {
    tab(depth);
    std::cout << "statement\n";
    if(contents) {
        contents->show(depth+1);
    }
    else {
        tab(depth+1);
        std::cout << "(none)\n";
    }
}

AssignmentStatementNode::AssignmentStatementNode(ParseTreeNode *identifier, ParseTreeNode *expression)
    :identifier(identifier), expression(expression) {}
AssignmentStatementNode::~AssignmentStatementNode() {
    delete identifier;
    delete expression;
}
void AssignmentStatementNode::show(int depth) {
    tab(depth);
    std::cout << "assignment_statement\n";
    identifier->show(depth+1);
    expression->show(depth+1);
}

PrintStatementNode::PrintStatementNode(ParseTreeNode *arguments)
    :arguments(arguments) {}
PrintStatementNode::~PrintStatementNode() {
    delete arguments;
}
void PrintStatementNode::show(int depth) {
    tab(depth);
    std::cout << "print_statement\n";
    arguments->show(depth+1);
}

PrintlnStatementNode::PrintlnStatementNode(ParseTreeNode *arguments)
    :arguments(arguments) {}
PrintlnStatementNode::~PrintlnStatementNode() {
    delete arguments;
}
void PrintlnStatementNode::show(int depth) {
    tab(depth);
    std::cout << "println_statement\n";
    arguments->show(depth+1);
}

IfStatementNode::IfStatementNode(ParseTreeNode *condition, ParseTreeNode *body)
    : condition(condition), body(body) {}

IfStatementNode::~IfStatementNode() {
    delete condition;
    delete body;
}

void IfStatementNode::show(int depth) {
    tab(depth);
    std::cout << "if_statement\n";
    condition->show(depth+1);
    body->show(depth+1);
}

IfElseStatementNode::IfElseStatementNode(ParseTreeNode* condition, ParseTreeNode* if_body, ParseTreeNode* else_body)
    : condition(condition), if_body(if_body), else_body(else_body) {}

IfElseStatementNode::~IfElseStatementNode() {
    delete condition;
    delete if_body;
    delete else_body;
}

void IfElseStatementNode::show(int depth) {
    tab(depth);
    std::cout << "if_else_statement\n";
    condition->show(depth + 1);
    if_body->show(depth + 1);
    else_body->show(depth + 1);
}

WhileStatementNode::WhileStatementNode(ParseTreeNode* condition, ParseTreeNode* body)
    : condition(condition), body(body) {}

WhileStatementNode::~WhileStatementNode() {
    delete condition;
    delete body;
}

void WhileStatementNode::show(int depth) {
    tab(depth);
    std::cout << "while_statement\n";
    condition->show(depth + 1);
    body->show(depth + 1);
}


ActualArgsNode::ActualArgsNode() {
    expressions = new std::vector<ParseTreeNode*>();
}

ActualArgsNode::ActualArgsNode(ParseTreeNode* arg) {
    expressions = new std::vector<ParseTreeNode*>();
    expressions->push_back(arg);
}

ActualArgsNode::~ActualArgsNode() {
    for (ParseTreeNode* arg : *expressions) {
        delete arg;
    }
    delete expressions;
}

void ActualArgsNode::append(ParseTreeNode* arg) {
    expressions->push_back(arg);
}

void ActualArgsNode::show(int depth) {
    tab(depth);
    std::cout << "actual_args\n";
    if (!expressions->empty()) {
        for (ParseTreeNode* arg : *expressions) {
            arg->show(depth + 1);
        }
    } else {
        tab(depth + 1);
        std::cout << "(none)\n";
    }
}

CallNode::CallNode(ParseTreeNode* identifier, ParseTreeNode* arguments)
    : identifier(identifier), arguments(arguments) {}

CallNode::~CallNode() {
    delete identifier;
    delete arguments;
}

void CallNode::show(int depth) {
    tab(depth);
    std::cout << "func_call\n";
    identifier->show(depth + 1);
    arguments->show(depth + 1);
}

ReturnNode::ReturnNode() : expression(nullptr) {}

ReturnNode::ReturnNode(ParseTreeNode *expression) : expression(expression) {}

ReturnNode::~ReturnNode() {
    delete expression;
}

void ReturnNode::show(int depth) {
    tab(depth);
    std::cout << "return_statement" << std::endl;
    if (expression) {
        expression->show(depth + 1);
    }
    else {
        tab(depth+1);
        std::cout << "(none)\n";
    }
}

UnaryNode::UnaryNode(Operator op, ParseTreeNode* expression)
    : op(op), expression(expression) {}
UnaryNode::~UnaryNode() {
    delete expression;
}
void UnaryNode::show(int depth) {
    tab(depth);
    std::cout << "unary: (" << operatorToString(op) << ")\n";
    expression->show(depth + 1);
}

BinaryNode::BinaryNode(Operator op, ParseTreeNode* left, ParseTreeNode* right)
    : op(op), left(left), right(right) {}

BinaryNode::~BinaryNode() {
    delete left;
    delete right;
}

void BinaryNode::show(int depth) {
    tab(depth);
    std::cout << "binary: (" << operatorToString(op) << ")\n";
    left->show(depth + 1);
    right->show(depth + 1);
}

NumberNode::NumberNode(int value)
    :value(value) {}
NumberNode::~NumberNode() {}
void NumberNode::show(int depth) {
    tab(depth);
    std::cout << "number: " << value << '\n';
}

BoolNode::BoolNode(bool value)
    :value(value) {}
BoolNode::~BoolNode() {}
void BoolNode::show(int depth) {
    tab(depth);
    std::cout << "bool: " << (value ? "true" : "false") << '\n';
}

IdentifierNode::IdentifierNode(std::string id):
    identifier(id) {}
IdentifierNode::~IdentifierNode() {}
void IdentifierNode::show(int depth) {
    tab(depth);
    std::cout << "identifier: " << identifier << '\n';
}