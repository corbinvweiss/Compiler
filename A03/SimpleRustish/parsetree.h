/*
parsetree.h
Corbin Weiss
2025-3-12

Outline the structure of the parse tree that can print itself as outlined in
https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/parser.html
*/

#pragma once

class ParseTreeNode {
    public:
        ParseTreeNode();
        virtual ~ParseTreeNode();
        virtual void show(int depth) = 0;
    };

class MainDefNode: public ParseTreeNode {
    protected:
        ParseTreeNode *body;
    public: 
        MainDefNode(ParseTreeNode *body);
        virtual ~MainDefNode();
        void show(int depth) override;  // calls the show function for its body
};

class FuncBodyNode: public ParseTreeNode {
    // todo: add declaration list and statement list here
    public:
        FuncBodyNode();
        void show(int depth) override;  // shows itself.
};