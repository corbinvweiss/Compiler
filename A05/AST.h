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
        virtual void EmitCode() = 0;
};

class NumberNode: public ASTNode {
    private:
        int value;
    public:
        NumberNode(int value, ErrorData err);
        ~NumberNode();
        int getValue();
        void EmitCode() override; // Emit code for a number literal
};

class BoolNode: public ASTNode {
    private:
        bool value;
    public:
        BoolNode(bool value, ErrorData err);
        ~BoolNode();
        bool getValue();
        void EmitCode() override; // Emit code for a boolean literal
};

class TypeNode: public ASTNode {
    public:
        TypeNode(TypeInfo t, ErrorData err);
        ~TypeNode();
        void EmitCode() override; // Emit code for a type node
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
        void EmitCode() override; // Emit code for an array literal
};

class LValueNode: public ASTNode {
    public:
        virtual void initialize() = 0; // initialize the identifier or array
        LValueNode(ErrorData err): ASTNode(err) {}
        void EmitCode() override = 0; // Emit code for an l-value
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
        void EmitCode() override; // Emit code for an identifier
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
        void EmitCode() override; // Emit code for array access
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
        void EmitCode() override; // Emit code for variable declaration
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
        void EmitCode() override; // Emit code for array declaration
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
        void EmitCode() override; // Emit code for assignment statement
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
        void EmitCode() override; // Emit code for a list of statements
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
        void EmitCode() override; // Emit code for local declarations
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
        void EmitCode() override; // Emit code for the main function
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
        void EmitCode() override; // Emit code for parameters list
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
        void EmitCode() override; // Emit code for function definition
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
        void EmitCode() override; // Emit code for return statement
};

class ActualArgsNode: public ASTNode {
    private:
        std::vector<ASTNode*>* actual_args;
    public:
        ActualArgsNode(ErrorData err);
        ActualArgsNode(ASTNode* arg, ErrorData err);
        ~ActualArgsNode();
        void append(ASTNode* arg);
        int getSize();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        std::vector<TypeInfo> argTypes();
        void EmitCode() override; // Emit code for actual arguments
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
        void EmitCode() override; // Emit code for function call
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
        void EmitCode() override; // Emit code for if statement
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
        void EmitCode() override; // Emit code for while statement
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
    void EmitCode() override; // Emit code for print statement
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
        void EmitCode() override; // Emit code for unary operation
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
        void EmitCode() override; // Emit code for binary operation
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
        void EmitCode() override; // Emit code for function definitions list
};


class ProgramNode : public ASTNode {
    private:
        MainDefNode* main_def;
        FuncDefListNode* func_def_list;
    public:
        ProgramNode(ASTNode* func_list, ASTNode* main, FILE* fdout);
        ~ProgramNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void EmitCode() override; // Emit code for the program
};




