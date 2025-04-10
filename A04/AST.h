/*
AST.h
Corbin Weiss
27 March 2025
Define the classes used in the Abstract Syntax Tree for Rustish
*/

/*
*** Outline of Approach ***
Every ASTNode has a type, a line number, and a pointer to its local and global symbol table
The Program owns the global symbol table, and each function owns its own local symbol table
The owners of the symbol tables create them and share them with their children using setLocalST

*/
#pragma once
#include <vector>
#include <string>
#include "SymbolTable.h"
#include <iostream>

// Operators used to check types of operands
static std::unordered_map<std::string, Type> Operators = 
{
    {"+", Type::i32},
    {"-", Type::i32},
    {"*", Type::i32},
    {"/", Type::i32},
    {"&&", Type::Bool},
    {"||", Type::Bool},
    {"==", Type::none}, // equality works on bools and i32s so we pass none as the type
    {"!=", Type::none}, // not eq also works on bools and i32s
    {"<=", Type::i32},
    {">=", Type::i32},
    {"<", Type::i32},
    {">", Type::i32},
};

class ASTNode {
    private:
        Type _type = Type::none;
    public:
        int lineno;
        SymbolTable* GlobalST;
        SymbolTable* LocalST;

        ASTNode(int line);
        virtual ~ASTNode() = default;
        virtual void setGlobalST(SymbolTable* ST) {};
        virtual void setLocalST(SymbolTable* ST) {};
        virtual void TypeCheck() {};

        virtual void setType(Type t) {
            _type = t;
        }
        virtual Type getType() {
            return _type;
        }
        
};

class LiteralNode: public ASTNode {
    private:
        Literal* value = new Literal();
    public:
        LiteralNode(int value, int line);
        LiteralNode(bool value, int line);
        ~LiteralNode();
};

class TypeNode: public ASTNode {
    public:
        TypeNode(Type t, int line);
        ~TypeNode();
};

class IdentifierNode: public ASTNode {
    private:
        std::string lexeme;
    public:
        std::string get_lexeme();
        IdentifierNode(std::string id, int line);
        ~IdentifierNode();
        Type getType() override;
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};

class VarDeclNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        TypeNode* type;
    public:
        VarDeclNode(ASTNode* id, ASTNode* t, int line);
        ~VarDeclNode();
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};

class AssignmentStatementNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        ASTNode* expression;
    public:
        AssignmentStatementNode(ASTNode* identifier, ASTNode* expr, int line);
        ~AssignmentStatementNode();
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};

class StatementListNode: public ASTNode {
    private:
        std::vector<ASTNode*> * stmt_list;
    public:
        StatementListNode(int line);
        ~StatementListNode();
        void TypeCheck() override;
        void append(ASTNode* stmt);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};

class LocalDeclListNode: public ASTNode {
    private:
        std::vector<VarDeclNode*> * decl_list;
    public:
        LocalDeclListNode(int lineno);
        LocalDeclListNode(ASTNode* decl, int line);
        ~LocalDeclListNode();
        void TypeCheck() override;
        void append(ASTNode* decl);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};

class MainDefNode: public ASTNode {
    private:
        LocalDeclListNode* local_decl_list;
        StatementListNode* stmt_list;
    public:
        MainDefNode(ASTNode* local_decl_list, ASTNode* stmt_list, int line);
        ~MainDefNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
};

class ParamsListNode: public ASTNode {
    private: 
        std::vector<VarDeclNode*>* parameters;
    public:
        ParamsListNode(int line);
        ParamsListNode(ASTNode* param, int line);
        ~ParamsListNode();
        void append(ASTNode* parameter);
        std::vector<Type> getTypes();   // return the types of the parameters
        // note: the parameters of a function do not need symbol tables
};

class FuncDefNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        ParamsListNode* params_list;
        TypeNode*       return_type;
        LocalDeclListNode* local_decl_list;
        StatementListNode* stmt_list;
    public:
        FuncDefNode(ASTNode* id, ASTNode* params, ASTNode* type, ASTNode* decl_list, ASTNode* stmt_list, int line);
        ~FuncDefNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
        void CheckReturn();
};

class ActualArgsNode: public ASTNode {
    private:
        std::vector<ASTNode*>* actual_args;
    public:
        ActualArgsNode(int line);
        ActualArgsNode(ASTNode* arg, int line);
        ~ActualArgsNode();
        void append(ASTNode* arg);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        std::vector<Type> argTypes();
};

class CallNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        ActualArgsNode* actual_args;
    public:
        CallNode(ASTNode* id, ASTNode* act_args, int line);
        ~CallNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        Type getType() override;
        void TypeCheck() override;
};

class BinaryNode : public ASTNode {
    private:
        std::string op;
        ASTNode* left;
        ASTNode* right;
    public:
        BinaryNode(std::string op, ASTNode* l, ASTNode* r, int line);
        ~BinaryNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
};

class FuncDefListNode: public ASTNode {
    private:
        std::vector<FuncDefNode*>* func_def_list;
    public:
        FuncDefListNode(int line);
        ~FuncDefListNode();
        void append(ASTNode* func_def);
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        // note: the list of function def's do not exist in a local symbol table 
};


class ProgramNode : public ASTNode {
    private:
        MainDefNode* main_def;
        FuncDefListNode* func_def_list;
    public:
        ProgramNode(ASTNode* func_list, ASTNode* main);
        ~ProgramNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};




