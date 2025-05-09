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
#include <stack> // Include stack for std::stack

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

void begin_func(std::string name);
void end_func(std::string name);

struct LabelTracker {
    int if_count;       // stacked counter for nested ifs
    std::stack<int> if_stack; 
    int while_count;    // stacked counter for nested whiles
    std::stack<int> while_stack;
    int counter;        // basic counter for all other needs
    LabelTracker();
    void Label(const char* l);
    void BranchElse(const char* reg);
    void JumpEndIf();
    void EndIfLabel();
    void ElseLabel();
    void BeginWhileLabel();
    void BranchWhile(const char* reg);
    void JumpBeginWhile();
    void EndWhileLabel();
};

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
        virtual bool TypeCheck() { return true; };
        virtual std::vector<ASTNode*> FindReturns() {return {};}

        virtual void setType(TypeInfo t) {
            _type = t;
        }
        virtual TypeInfo getType() {
            return _type;
        }
        virtual void EmitCode(LabelTracker&) = 0;
};

class NumberNode: public ASTNode {
    private:
        int value;
    public:
        NumberNode(int value, ErrorData err);
        ~NumberNode();
        int getValue();
        void EmitCode(LabelTracker&) override; // Emit code for a number literal
};

class BoolNode: public ASTNode {
    private:
        bool value;
    public:
        BoolNode(bool value, ErrorData err);
        ~BoolNode();
        bool getValue();
        void EmitCode(LabelTracker&) override; // Emit code for a boolean literal
};

class CharNode: public ASTNode {
    private:
        char value;
    public:
        CharNode(std::string val, ErrorData err);
        ~CharNode();
        char getValue();
        void EmitCode(LabelTracker&) override;
};

class StringNode: public ASTNode {
    private:
        std::string value;
    public:
        StringNode(std::string val, ErrorData err);
        ~StringNode();
        void EmitCode(LabelTracker&) override;
};

class TypeNode: public ASTNode {
    public:
        TypeNode(TypeInfo t, ErrorData err);
        ~TypeNode();
        void EmitCode(LabelTracker&) override; // Emit code for a type node
};

// an array literal is just a list of expressions
class ArrayLiteralNode: public ASTNode {
    private:
        std::vector<ASTNode*>* expressions;
    public:
        ArrayLiteralNode(ASTNode* expression, ErrorData err);
        ~ArrayLiteralNode();
        void append(ASTNode* expression);
        bool TypeCheck() override;
        void EmitCode(LabelTracker&) override; // Emit code for an array literal
};

class LValueNode: public ASTNode {
    public:
        virtual void Initialize() = 0; // initialize the identifier or array
        LValueNode(ErrorData err): ASTNode(err) {}
        virtual std::string getLexeme() = 0;
        void EmitCode(LabelTracker&) override = 0; // Emit code for an l-value
        virtual void EmitSetCode(LabelTracker&) = 0;
};

class IdentifierNode: public LValueNode {
    private:
        std::string lexeme;
    public:
        IdentifierNode(std::string id, ErrorData err);
        IdentifierNode(IdentifierNode& cp); 
        ~IdentifierNode();
        std::string getLexeme() override;
        TypeInfo getType() override;
        bool TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void Initialize() override;
        void EmitCode(LabelTracker&) override; // Emit code for an identifier
        void EmitSetCode(LabelTracker&) override;   // Emit code for set identifier value
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
        bool TypeCheck() override;
        std::string getLexeme() override;
        void Initialize() override;
        void EmitCode(LabelTracker&) override; // Emit code for get array access
        void EmitSetCode(LabelTracker&) override;   // Emit code for set array access
        void Access(LabelTracker&);
};

class VarDeclNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        TypeNode* type;
    public:
        VarDeclNode(ASTNode* id, ASTNode* t, ErrorData err);
        ~VarDeclNode();
        bool TypeCheck() override;
        void Initialize();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void EmitCode(LabelTracker&) override; // Emit code for variable declaration
};

class ArrayDeclNode: public ASTNode {
    private:
        IdentifierNode* identifier;
        TypeNode* type;
        NumberNode* length;
    public:
        ArrayDeclNode(ASTNode* id, ASTNode* tp, ASTNode* len, ErrorData err);
        ~ArrayDeclNode();
        bool TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void EmitCode(LabelTracker&) override; // Emit code for array declaration
};

class AssignmentStatementNode: public ASTNode {
    private:
        LValueNode* identifier;
        ASTNode* expression;
    public:
        AssignmentStatementNode(ASTNode* identifier, ASTNode* expr, ErrorData err);
        ~AssignmentStatementNode();
        bool TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void EmitCode(LabelTracker&) override; // Emit code for assignment statement
};

class StatementListNode: public ASTNode {
    private:
        std::vector<ASTNode*> * stmt_list;
    public:
        StatementListNode(ErrorData err);
        ~StatementListNode();
        bool TypeCheck() override;
        void append(ASTNode* stmt);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        std::vector<ASTNode*> FindReturns() override;
        void EmitCode(LabelTracker&) override; // Emit code for a list of statements
};

class LocalDeclListNode: public ASTNode {
    private:
        std::vector<VarDeclNode*> * decl_list;
    public:
        LocalDeclListNode(ErrorData err);
        LocalDeclListNode(ASTNode* decl, ErrorData err);
        ~LocalDeclListNode();
        bool TypeCheck() override;
        void append(ASTNode* decl);
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        void EmitCode(LabelTracker&) override; // Emit code for local declarations
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
        bool TypeCheck() override;
        void EmitCode(LabelTracker&) override; // Emit code for the main function
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
        int getSize() { return parameters->size(); }
        // note: the parameters of a function do not need a global symbol table
        bool TypeCheck() override;      // populate the parameters into the local symbol table
        void EmitCode(LabelTracker&) override; // Emit code for parameters list
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
        bool TypeCheck() override;
        bool CheckReturn();
        void EmitCode(LabelTracker&) override; // Emit code for function definition
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
        bool TypeCheck() override;
        std::vector<ASTNode*> FindReturns() override;
        void EmitCode(LabelTracker&) override; // Emit code for return statement
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
        std::vector<ASTNode*>* getArgs() { return actual_args; }
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        std::vector<TypeInfo> argTypes();
        void EmitCode(LabelTracker&) override; // Emit code for actual arguments
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
        bool TypeCheck() override;
        void EmitCode(LabelTracker&) override; // Emit code for function call
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
        bool TypeCheck() override;
        std::vector<ASTNode*> FindReturns() override;
        void EmitCode(LabelTracker&) override; // Emit code for if statement
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
        bool TypeCheck() override;
        std::vector<ASTNode*> FindReturns() override;
        void EmitCode(LabelTracker&) override; // Emit code for while statement
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
        bool TypeCheck() override;
        void EmitCode(LabelTracker&) override; // Emit code for print statement
};

class ReadNode: public ASTNode {
    public:
        ReadNode(ErrorData err);
        ~ReadNode();
        bool TypeCheck() override;
        void EmitCode(LabelTracker&) override;
};

class LengthNode: public ASTNode {
    private:
        IdentifierNode* identifier;
    public:
        LengthNode(ASTNode* id, ErrorData err);
        ~LengthNode();
        void setGlobalST(SymbolTable* ST) override;
        void setLocalST(SymbolTable* ST) override;
        bool TypeCheck() override;
        void EmitCode(LabelTracker&) override;
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
        bool TypeCheck() override;
        void EmitCode(LabelTracker&) override; // Emit code for unary operation
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
        bool TypeCheck() override;
        bool BoolInt(TypeInfo t);
        void EmitCode(LabelTracker&) override; // Emit code for binary operation
};

class FuncDefListNode: public ASTNode {
    private:
        std::vector<FuncDefNode*>* func_def_list;
    public:
        FuncDefListNode(ErrorData err);
        ~FuncDefListNode();
        void append(ASTNode* func_def);
        bool TypeCheck() override;
        void setGlobalST(SymbolTable* ST) override;
        // note: the list of function def's do not exist in a local symbol table 
        void EmitCode(LabelTracker&) override; // Emit code for function definitions list
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
        void EmitCode(LabelTracker&) override; // Emit code for the program
};




