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
    public:
        MainDefNode();
        ~MainDefNode();
};
