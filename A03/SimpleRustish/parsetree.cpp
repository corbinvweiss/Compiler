/*
parsetree.cpp
Corbin Weiss
2025-3-12

implement a parse tree that can print itself as outlined in 
https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/parser.html
*/

#include <iostream>
#include "parsetree.h"

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

FuncBodyNode::FuncBodyNode(ParseTreeNode *local_decl_list) 
    :local_decl_list(local_decl_list) {}
FuncBodyNode::~FuncBodyNode() {
    delete local_decl_list;
}
void FuncBodyNode::show(int depth) {
    tab(depth);
    std::cout << "func_body\n";
    local_decl_list->show(depth+1);
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
    for (ParseTreeNode *declaration : *local_decl_list) {
        declaration->show(depth+1);
    }
}

IdentifierNode::IdentifierNode(std::string id):
    identifier(id) {}
IdentifierNode::~IdentifierNode() {}
void IdentifierNode::show(int depth) {
    tab(depth);
    std::cout << "identifier: " << identifier << '\n';
}