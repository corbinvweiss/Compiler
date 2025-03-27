/*
AST.h
Corbin Weiss
27 March 2025
Define the classes used in the Abstract Syntax Tree for Rustish

*/
#pragma once
#include <vector>
#include <string>
#include "SymbolTable.h"

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

class IdentifierNode: public ASTNode {
    private:
        std::string lexeme;
    public:
        std::string get_lexeme();
        IdentifierNode(std::string id);
        ~IdentifierNode();
};

class TypeNode: public ASTNode {
    public:
        TypeNode(Type t);
        ~TypeNode();
};

class VarDeclNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        TypeNode* type;
    public:
        VarDeclNode(ASTNode* id, ASTNode* t);
        ~VarDeclNode();
        void UpdateSymbolTable(SymbolTable* symbol_table);
};

class LocalDeclListNode: public ASTNode {
    private:
        std::vector<VarDeclNode*> * decl_list;
    public:
        LocalDeclListNode();
        LocalDeclListNode(ASTNode* decl);
        void UpdateSymbolTable(SymbolTable* symbol_table);
        void append(ASTNode* decl);
        ~LocalDeclListNode();
};

class MainDefNode: public ASTNode {
    private:
        LocalDeclListNode* local_decl_list;
        SymbolTable* symbol_table;
    public:
        MainDefNode(ASTNode* local_decl_list);
        ~MainDefNode();
};

class ProgramNode : public ASTNode {
    private:
        MainDefNode* main_def;
        SymbolTable* symbol_table;
    public:
        ProgramNode(ASTNode *main);
        ~ProgramNode();
};




