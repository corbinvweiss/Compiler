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

enum Type {
    NONE,
    _I32,
    _BOOL,
    ARRAY_I32,
    ARRAY_BOOL
};

enum Operator {
    _PLUS,
    _MINUS,
    _TIMES,
    _DIVIDE,
    _MODULUS,
    _AND,
    _OR,
    _EQ,
    _NE,
    _LT,
    _LE,
    _GT,
    _GE,
    _NOT
};


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
    // 1. with parameters and return type
    // 2. with parameters and no return type
    protected:
        ParseTreeNode *identifier;
        ParseTreeNode *parameters;
        ParseTreeNode *returntype;
        ParseTreeNode *body;
    public:
        FuncDefNode(ParseTreeNode *identifier, ParseTreeNode *parameters, ParseTreeNode *returntype, ParseTreeNode *body);
        FuncDefNode(ParseTreeNode *identifier, ParseTreeNode *parameters, ParseTreeNode *body);
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

class ParamsListNode: public ParseTreeNode {
    protected:
        std::vector<ParseTreeNode *> *params_list;
    public:
        ParamsListNode(ParseTreeNode *param);
        ParamsListNode();
        ~ParamsListNode();
        void append(ParseTreeNode* var_decl);
        void show(int depth) override;
};

class VarDeclNode: public ParseTreeNode {
    protected:
        ParseTreeNode *identifier;
        ParseTreeNode *type;
    public:
        VarDeclNode(ParseTreeNode* identifier, ParseTreeNode *type);
        ~VarDeclNode();
        void show(int depth) override;
};

class TypeNode: public ParseTreeNode {
    protected:
        Type type;
    public:
        TypeNode(Type type);
        ~TypeNode();
        void show(int depth) override;
};

class FuncBodyNode: public ParseTreeNode {
    private:
        ParseTreeNode *local_decl_list;
        ParseTreeNode *statement_list;
    public:
        FuncBodyNode(ParseTreeNode *local_decl_list, ParseTreeNode *statement_list);
        ~FuncBodyNode();
        void show(int depth) override;  // shows itself.
};

class LocalDeclListNode: public ParseTreeNode {
    private:
        std::vector<ParseTreeNode *> *local_decl_list;
    public:
        LocalDeclListNode();
        ~LocalDeclListNode();
        void append(ParseTreeNode *declaration);
        void show(int depth) override;
};

class StatementListNode: public ParseTreeNode {
    private:
        std::vector<ParseTreeNode *> *statement_list;
    public:
        StatementListNode();
        StatementListNode(ParseTreeNode *statement);
        ~StatementListNode();
        void append(ParseTreeNode *statement);
        void show(int depth) override;
};

class StatementNode: public ParseTreeNode {
    private:
        ParseTreeNode *contents;
    public:
        StatementNode();
        StatementNode(ParseTreeNode *contents);
        ~StatementNode();
        void show(int depth) override;
};

class AssignmentStatementNode: public ParseTreeNode {
    private:
        ParseTreeNode *identifier;
        ParseTreeNode *expression;
    public:
        AssignmentStatementNode(ParseTreeNode *identifier, ParseTreeNode *expression);
        ~AssignmentStatementNode();
        void show(int depth) override;
};

class PrintStatementNode: public ParseTreeNode {
    private:
        ParseTreeNode *arguments;
    public:
        PrintStatementNode(ParseTreeNode *arguments);
        ~PrintStatementNode();
        void show(int depth) override;
};

class PrintlnStatementNode: public ParseTreeNode {
    private:
        ParseTreeNode *arguments;
    public:
        PrintlnStatementNode(ParseTreeNode *arguments);
        ~PrintlnStatementNode();
        void show(int depth) override;
};

class ActualArgsNode: public ParseTreeNode {
    private:
        std::vector<ParseTreeNode *> *expressions;
    public:
        ActualArgsNode();
        ActualArgsNode(ParseTreeNode *expression);
        ~ActualArgsNode();
        void append(ParseTreeNode *expression);
        void show(int depth) override;
};

class UnaryNode: public ParseTreeNode {
    private:
        Operator op;
        ParseTreeNode *expression;
    public:
        UnaryNode(Operator op, ParseTreeNode *expression);
        ~UnaryNode();
        void show(int depth) override;
};

class NumberNode: public ParseTreeNode {
    protected:
        int value;
    public:
        NumberNode(int value);
        ~NumberNode();
        void show(int depth) override;
};

class BoolNode: public ParseTreeNode {
    protected:
        bool value;
    public:
        BoolNode(bool value);
        ~BoolNode();
        void show(int depth) override;
};

class IdentifierNode: public ParseTreeNode {
    protected:
        std::string identifier;
    public:
        IdentifierNode(std::string value);
        ~IdentifierNode();
        void show(int depth) override;
};;