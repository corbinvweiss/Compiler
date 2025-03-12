/*
parsetree.h
Corbin Weiss
2025-3-12

Outline the structure of the parse tree that can print itself as outlined in
https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/parser.html
*/

#pragma once
#include <vector>
#include <string>

class ParseTreeNode {
    public:
        ParseTreeNode();
        virtual ~ParseTreeNode();
        virtual void show(int depth) = 0;
    };


class ProgramNode: public ParseTreeNode {
    protected:
        ParseTreeNode* func_def_list;
        ParseTreeNode* main_def;
    public:
        ProgramNode(ParseTreeNode*, ParseTreeNode*);
        ~ProgramNode();
        void show(int depth) override;
};

class FuncDefListNode: public ParseTreeNode {
    protected:
        std::vector<ParseTreeNode *> *func_def_list;
    public:
        // the constructor adds another function to the func_def_list
        FuncDefListNode();
        ~FuncDefListNode();
        void append(ParseTreeNode* func_def);
        void show(int depth) override;
};

class FuncDefNode: public ParseTreeNode {
    protected:
        ParseTreeNode *identifier;
        ParseTreeNode *body;
    public:
        FuncDefNode(ParseTreeNode *identifier, ParseTreeNode *body);
        ~FuncDefNode();
        void show(int depth) override;
};

class MainDefNode: public ParseTreeNode {
    protected:
        ParseTreeNode *body;
    public: 
        MainDefNode(ParseTreeNode *body);
        ~MainDefNode();
        void show(int depth) override;  // calls the show function for its body
};

class FuncBodyNode: public ParseTreeNode {
    // todo: add declaration list and statement list here
    public:
        FuncBodyNode();
        ~FuncBodyNode();
        void show(int depth) override;  // shows itself.
};

class IdentifierNode: public ParseTreeNode {
    protected:
        std::string identifier;
    public:
        IdentifierNode(std::string value);
        ~IdentifierNode();
        void show(int depth) override;
};