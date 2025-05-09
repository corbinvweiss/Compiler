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
#include "ErrorData.h"

struct OpType {
    TypeInfo op_type;
    TypeInfo return_type;
    OpType(TypeInfo op, TypeInfo re)
        :op_type(op), return_type(re) {}
};

// Operators used to check types of operands
static std::unordered_map<std::string, OpType> Operators = 
{
    {"+", OpType(Type::i32, Type::i32)},
    {"-", OpType(Type::i32, Type::i32)},
    {"*", OpType(Type::i32, Type::i32)},
    {"/", OpType(Type::i32, Type::i32)},
    {"%", OpType(Type::i32, Type::i32)},
    {"&&", OpType(Type::Bool, Type::Bool)},
    {"||", OpType(Type::Bool, Type::Bool)},
    {"==", OpType(Type::any, Type::Bool)}, // equality works on bools and i32s
    {"!=", OpType(Type::any, Type::Bool)}, // not eq also works on bools and i32s
    {"<=", OpType(Type::i32, Type::Bool)},
    {">=", OpType(Type::i32, Type::Bool)},
    {"<", OpType(Type::i32, Type::Bool)},
    {">", OpType(Type::i32, Type::Bool)},
    {"!", OpType(Type::Bool, Type::Bool)},
};

static OpType GetOpType(std::string op) {
    if(Operators.find(op) != Operators.end()) {
        return Operators.find(op)->second;
    }
    else return OpType(Type::none, Type::none);
}

// TODO: add return types from each of these

class ASTNode {
    private:
        TypeInfo _type = Type::none;
    public:
        ErrorData err_data;
        SymbolTable* GlobalST;
        SymbolTable* LocalST;

        ASTNode(ErrorData);
        virtual ~ASTNode() = default;
        virtual void setGlobalST(SymbolTable* ST) {};
        virtual void setLocalST(SymbolTable* ST) {};
        virtual void TypeCheck() {};
        virtual ASTNode* FindReturn() {return nullptr;}

        virtual void setType(TypeInfo t) {
            _type = t;
        }
        virtual TypeInfo getType() {
            return _type;
        }
        
};

class NumberNode: public ASTNode {
    private:
        int value;
    public:
        NumberNode(int value, ErrorData err);
        NumberNode(bool value, ErrorData err);
        ~NumberNode();
        int getValue();
};

class BoolNode: public ASTNode {
    private:
        bool value;
    public:
        BoolNode(int value, ErrorData err);
        BoolNode(bool value, ErrorData err);
        ~BoolNode();
        bool getValue();
};

class TypeNode: public ASTNode {
    public:
        TypeNode(TypeInfo t, ErrorData err);
        ~TypeNode();
};

// an array literal is just a list of expressions
class ArrayLiteralNode: public ASTNode {
    private:
        std::vector<ASTNode*>* expressions;
    public:
        ArrayLiteralNode(ASTNode* expression, ErrorData err);
        ~ArrayLiteralNode();
        void append(ASTNode* expression);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
};

class LValueNode: public ASTNode {
    public:
        virtual void initialize() = 0; // initialize the identifier or array
        LValueNode(ErrorData err): ASTNode(err) {}
};

class IdentifierNode: public LValueNode {
    private:
        std::string lexeme;
    public:
        IdentifierNode(std::string id, ErrorData err);
        ~IdentifierNode();
        std::string getLexeme();
        TypeInfo getType() override;
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void initialize() override;
};

class ArrayAccessNode: public LValueNode {
    private:
        IdentifierNode* identifier;
        ASTNode* expression;
    public:
        ArrayAccessNode(ASTNode* id, ASTNode* expr, ErrorData err);
        ~ArrayAccessNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        TypeInfo getType() override;
        void TypeCheck() override;
        void initialize() override;
};

class VarDeclNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        TypeNode* type;
    public:
        VarDeclNode(ASTNode* id, ASTNode* t, ErrorData err);
        ~VarDeclNode();
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};

class ArrayDeclNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        TypeNode* type;
        NumberNode* length;
    public:
        ArrayDeclNode(ASTNode* id, ASTNode* tp, ASTNode* len, ErrorData err);
        ~ArrayDeclNode();
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;

};

class AssignmentStatementNode: public ASTNode {
    private:
        LValueNode* identifier;
        ASTNode* expression;
    public:
        AssignmentStatementNode(ASTNode* identifier, ASTNode* expr, ErrorData err);
        ~AssignmentStatementNode();
        void TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
};

class StatementListNode: public ASTNode {
    private:
        std::vector<ASTNode*> * stmt_list;
    public:
        StatementListNode(ErrorData err);
        ~StatementListNode();
        void TypeCheck() override;
        void append(ASTNode* stmt);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        std::vector<ASTNode*> FindReturns();
};

class LocalDeclListNode: public ASTNode {
    private:
        std::vector<VarDeclNode*> * decl_list;
    public:
        LocalDeclListNode(ErrorData err);
        LocalDeclListNode(ASTNode* decl, ErrorData err);
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
        MainDefNode(ASTNode* local_decl_list, ASTNode* stmt_list, ErrorData err);
        ~MainDefNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
};

class ParamsListNode: public ASTNode {
    private: 
        std::vector<VarDeclNode*>* parameters;
    public:
        ParamsListNode(ErrorData err);
        ParamsListNode(ASTNode* param, ErrorData err);
        ~ParamsListNode();
        void append(ASTNode* parameter);
        std::vector<TypeInfo> getTypes();   // return the types of the parameters
        void setLocalST(SymbolTable* ST) override;
        // note: the parameters of a function do not need a global symbol table
        void TypeCheck() override;      // populate the parameters into the local symbol table
};

class FuncDefNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        ParamsListNode* params_list;
        TypeNode*       return_type;
        LocalDeclListNode* local_decl_list;
        StatementListNode* stmt_list;
    public:
        FuncDefNode(ASTNode* id, ASTNode* params, ASTNode* type, ASTNode* decl_list, ASTNode* stmt_list, ErrorData err);
        ~FuncDefNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
        void CheckReturn();
};

class ReturnNode: public ASTNode {
    private: 
        ASTNode* expression;
    public:
        ReturnNode(ASTNode* expr, ErrorData err);
        ReturnNode(ErrorData err);
        ~ReturnNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
        ASTNode* FindReturn() override;
};

class ActualArgsNode: public ASTNode {
    private:
        std::vector<ASTNode*>* actual_args;
    public:
        ActualArgsNode(ErrorData err);
        ActualArgsNode(ASTNode* arg, ErrorData err);
        ~ActualArgsNode();
        void append(ASTNode* arg);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        std::vector<TypeInfo> argTypes();
};

class CallNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        ActualArgsNode* actual_args;
    public:
        CallNode(ASTNode* id, ASTNode* act_args, ErrorData err);
        ~CallNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        TypeInfo getType() override;
        void TypeCheck() override;
};

class IfStatementNode: public ASTNode {
    private:
        ASTNode* expression;
        StatementListNode* if_branch;
        StatementListNode* else_branch;
    public:
        IfStatementNode(ASTNode* expr, ASTNode* if_, ASTNode* else_, ErrorData err);
        ~IfStatementNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
};

class WhileStatementNode: public ASTNode {
    private:
        ASTNode* expression;
        StatementListNode* body;
    public:
        WhileStatementNode(ASTNode* expr, ASTNode* body, ErrorData err);
        ~WhileStatementNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
};

class PrintStatementNode: public ASTNode {
    private:
        bool newline;
        ActualArgsNode* actual_args;
    public:
    PrintStatementNode(ASTNode* args, bool ln, ErrorData err);
    ~PrintStatementNode();
    void setGlobalST(SymbolTable* ST) override;
    void setLocalST(SymbolTable* ST) override;
    void TypeCheck() override;
};

class UnaryNode : public ASTNode {
    private:
        std::string op;
        ASTNode* right;
    public:
        UnaryNode(std::string op, ASTNode* r, ErrorData err);
        ~UnaryNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
};

class BinaryNode : public ASTNode {
    private:
        std::string op;
        ASTNode* left;
        ASTNode* right;
    public:
        BinaryNode(std::string op, ASTNode* l, ASTNode* r, ErrorData err);
        ~BinaryNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void TypeCheck() override;
        bool BoolInt(TypeInfo t);
};

class FuncDefListNode: public ASTNode {
    private:
        std::vector<FuncDefNode*>* func_def_list;
    public:
        FuncDefListNode(ErrorData err);
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




