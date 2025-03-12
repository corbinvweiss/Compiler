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

FuncDefNode::FuncDefNode(ParseTreeNode *identifier, ParseTreeNode *body) 
    :identifier(identifier), body(body) {}
FuncDefNode::~FuncDefNode() {
    delete identifier;
    delete body;
}
void FuncDefNode::show(int depth) {
    tab(depth);
    std::cout << "func_def\n";
    identifier->show(depth+1);
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

FuncBodyNode::FuncBodyNode() {}
FuncBodyNode::~FuncBodyNode() {}
void FuncBodyNode::show(int depth) {
    tab(depth);
    std::cout << "func_body\n";
}

IdentifierNode::IdentifierNode(std::string id):
    identifier(id) {}
IdentifierNode::~IdentifierNode() {}
void IdentifierNode::show(int depth) {
    tab(depth);
    std::cout << "identifier: " << identifier << '\n';
}