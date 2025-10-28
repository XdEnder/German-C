#pragma once
#include <string>
#include <vector>
#include <map>
#include "token.h"

class Lexer {
public:
    Lexer(const std::string& quelltext);
    std::vector<Token> alleTokens();

private:
    std::string m_quelltext;
    int m_position = 0;
    int m_zeile = 1;
    int m_spalte = 1;
    char m_aktuellesZeichen;

    static std::map<std::string, TokenType> keywords;
    static std::map<std::string, TokenType> literals;

    void vorruecken();
    char peek();
    bool isAtEnd();
    
    Token naechstesToken();
    
    Token erkenneZahl();
    Token erkenneIdentifierOderKeyword();
    Token erkenneString();
    void skipWhitespace();
    void skipKommentar();
    void skipMultiLineKommentar();
};