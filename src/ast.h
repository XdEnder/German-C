#pragma once

#include <string>
#include <vector>
#include <memory>
#include <any>

class Statement;
class Expression;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    int zeile = 1;
    int spalte = 1;
};

class Expression : public ASTNode {};

class Statement : public ASTNode {};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

class LiteralExpression : public Expression {
public:
    std::any wert;
    std::string typName;
    LiteralExpression(std::any v, std::string t) : wert(v), typName(t) {}
};

class IdentifierExpression : public Expression {
public:
    std::string name;
    IdentifierExpression(std::string n) : name(n) {}
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> links;
    std::string op;
    std::unique_ptr<Expression> rechts;

    BinaryExpression(std::unique_ptr<Expression> l, std::string o, std::unique_ptr<Expression> r)
        : links(std::move(l)), op(o), rechts(std::move(r)) {}
};

class UnaryExpression : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> rechts;

    UnaryExpression(std::string o, std::unique_ptr<Expression> r)
        : op(o), rechts(std::move(r)) {}
};

class CallExpression : public Expression {
public:
    std::unique_ptr<Expression> aufrufer;
    std::vector<std::unique_ptr<Expression>> argumente;
};

class VariableDeclaration : public Statement {
public:
    std::string typName;
    std::string name;
    std::unique_ptr<Expression> initializer;
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

class FunctionDeclaration : public Statement {
public:
    std::string returnTyp;
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameter;
    std::unique_ptr<BlockStatement> body;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> wert;
};

class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
};

class AssignmentStatement : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> wert;
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
};

class ForStatement : public Statement {
public:
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> increment;
    std::unique_ptr<Statement> body;
};

class ArrayLiteralExpression : public Expression {
public:
    std::vector<std::unique_ptr<Expression>> elemente;
};

class ArrayAccessExpression : public Expression {
public:
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;
};

class ArrayAssignmentStatement : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> index;
    std::unique_ptr<Expression> wert;
};

class LeseExpression : public Expression {
    // sie dient nur als "Markierung" für den Generator.
};

class TypeConversionExpression : public Expression {
public:
    std::string zielTyp;
    std::unique_ptr<Expression> expression;
};