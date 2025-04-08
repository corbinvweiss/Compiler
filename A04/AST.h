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
#include <iostream>

class ASTNode {
    private:
        Type _type = Type::none;
    public:
        int lineno;
        ASTNode(int line) :lineno(line) {};
        virtual ~ASTNode() = default;

        virtual void UpdateSymbolTable(SymbolTable* ST) {std::cout << "base class UpdateSymbolTable\n"; };

        virtual void set_type(Type t) {
            _type = t;
        }
        virtual Type get_type() {
            return _type;
        }
};

class LiteralNode: public ASTNode {
    private:
        Literal value = Literal();
    public:
        Literal get_value();
        LiteralNode(int value, int line);
        LiteralNode(bool value, int line);
        ~LiteralNode();
};

class IdentifierNode: public ASTNode {
    private:
        std::string lexeme;
    public:
        std::string get_lexeme();
        IdentifierNode(std::string id, int line);
        ~IdentifierNode();
};

class TypeNode: public ASTNode {
    public:
        TypeNode(Type t, int line);
        ~TypeNode();
};

class VarDeclNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        TypeNode* type;
    public:
        VarDeclNode(ASTNode* id, ASTNode* t, int line);
        ~VarDeclNode();
        void UpdateSymbolTable(SymbolTable* symbol_table) override;
};

class AssignmentStatementNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        LiteralNode* literal;
    public:
        AssignmentStatementNode(ASTNode* identifier, ASTNode* value, int line);
        ~AssignmentStatementNode();
        void UpdateSymbolTable(SymbolTable* ST) override;
};

class StatementListNode: public ASTNode {
    private:
        std::vector<ASTNode*> * stmt_list;
    public:
        StatementListNode(int line);
        ~StatementListNode();
        void UpdateSymbolTable(SymbolTable* symbol_table) override;
        void append(ASTNode* stmt);
};

class LocalDeclListNode: public ASTNode {
    private:
        std::vector<VarDeclNode*> * decl_list;
    public:
        LocalDeclListNode(int lineno);
        LocalDeclListNode(ASTNode* decl, int line);
        void UpdateSymbolTable(SymbolTable* symbol_table) override;
        void append(ASTNode* decl);
        ~LocalDeclListNode();
};

class MainDefNode: public ASTNode {
    private:
        LocalDeclListNode* local_decl_list;
        StatementListNode* stmt_list;
        SymbolTable* symbol_table;
    public:
        MainDefNode(ASTNode* local_decl_list, ASTNode* stmt_list, int line);
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




