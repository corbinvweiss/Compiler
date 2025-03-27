/*
AST.h
Corbin Weiss
27 March 2025
Define the classes used in the Abstract Syntax Tree for Rustish

*/
#pragma once
#include <vector>
#include <string>

enum Type {
    none,
    i32,
    Bool,
    array_i32,
    array_bool,
};

std::string typeToString(Type t);

class ASTNode {
    private:
        Type _type = Type::none;
    public:
        ASTNode();
        virtual ~ASTNode();
        virtual void set_type(Type t) {
            _type = t;
        }
        virtual Type get_type() {
            return _type;
        }
};

class ProgramNode : public ASTNode {
    private:
        ASTNode* main_def;
    public:
        ProgramNode(ASTNode *main_def);
        ~ProgramNode();
};

class MainDefNode: public ASTNode {
    private:
        ASTNode* func_body;
    public:
        MainDefNode(ASTNode* func_body);
        ~MainDefNode();
};

class FuncBodyNode: public ASTNode {
    private:
        ASTNode* local_decl_list;
    public:
        FuncBodyNode(ASTNode* local_decl_list);
        ~FuncBodyNode();
};

class LocalDeclListNode: public ASTNode {
    private:
        std::vector<ASTNode*> * decl_list;
    public:
        LocalDeclListNode();
        LocalDeclListNode(ASTNode* decl);
        void append(ASTNode* decl);
        ~LocalDeclListNode();
};

class VarDeclNode: public ASTNode {
    private:
        ASTNode* identifier;
        ASTNode* type;
    public:
        VarDeclNode(ASTNode* id, ASTNode* t);
        ~VarDeclNode();
};

class IdentifierNode: public ASTNode {
    private:
        std::string lexeme;
    public:
        IdentifierNode(std::string id);
        ~IdentifierNode();
};

class TypeNode: public ASTNode {
    public:
        TypeNode(Type t);
        ~TypeNode();
};