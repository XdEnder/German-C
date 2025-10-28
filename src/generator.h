#pragma once
#include <string>
#include <sstream>
#include "ast.h"

class Generator {
public:
    Generator(std::unique_ptr<Program> program);
    
    std::string generate();

private:
    std::unique_ptr<Program> m_program;
    std::stringstream m_output;

    void visit(Statement* stmt);
    void visit(Expression* expr);

    void visitProgram(Program* p);
    void visitFunctionDeclaration(FunctionDeclaration* fd);
    void visitBlockStatement(BlockStatement* bs);
    void visitPrintStatement(PrintStatement* ps);
    void visitReturnStatement(ReturnStatement* rs);
    void visitIfStatement(IfStatement* is);
    void visitVariableDeclaration(VariableDeclaration* vd);
    void visitAssignmentStatement(AssignmentStatement* as);
    void visitWhileStatement(WhileStatement* ws);
    void visitForStatement(ForStatement* fs);
    void visitArrayAssignmentStatement(ArrayAssignmentStatement* aas);
    void visitLiteralExpression(LiteralExpression* le);
    void visitIdentifierExpression(IdentifierExpression* ie);
    void visitBinaryExpression(BinaryExpression* be);
    void visitUnaryExpression(UnaryExpression* ue);
    void visitCallExpression(CallExpression* ce);
    void visitArrayLiteralExpression(ArrayLiteralExpression* al);
    void visitArrayAccessExpression(ArrayAccessExpression* aa);
    void visitLeseExpression(LeseExpression* le);
    void visitTypeConversionExpression(TypeConversionExpression* tce);
    
    std::string mapType(const std::string& gerlangType);
};