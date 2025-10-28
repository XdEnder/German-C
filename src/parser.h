#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include "token.h"
#include "ast.h"

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();

private:
    std::vector<Token> m_tokens;
    int m_current = 0;

    Token& peek();
    Token& previous();
    Token& advance();
    bool isAtEnd();
    bool check(TokenType typ);
    bool match(std::vector<TokenType> typen);
    Token& consume(TokenType typ, const std::string& errorMsg);

    std::string parseType();

    std::unique_ptr<Statement> declaration();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> functionDeclaration();
    std::unique_ptr<Statement> variableDeclaration();
    std::unique_ptr<BlockStatement> blockStatement();
    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<ReturnStatement> returnStatement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> assignmentStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> forStatement();
    std::unique_ptr<Statement> arrayAssignmentStatement();

    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();
};