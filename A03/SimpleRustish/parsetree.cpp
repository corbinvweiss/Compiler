/*
parsetree.cpp
Corbin Weiss
2025-3-12

implement a parse tree that can print itself as outlined in 
https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/parser.html
*/

#include <iostream>
#include "parsetree.h"

ParseTreeNode::ParseTreeNode() {}
ParseTreeNode::~ParseTreeNode() {}

// print spaces to indent nodes in the tree
void tab(int depth) {
    for(int i=0; i<depth; i++) {
        std::cout << "    ";
    }
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
void FuncBodyNode::show(int depth) {
    tab(depth);
    std::cout << "func_body\n";
}