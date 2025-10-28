#pragma once
#include <string>
#include <iostream>

enum class TokenType {
    GANZ, KOMMA, WORT, JAIN, KISTE,
    WENN, SONST, SOLANGE, FUER,
    UND, ODER, NICHT,
    DRUCKE, LESE, ZURUECK,
    ZU_GANZ, ZU_KOMMA, ZU_WORT, 
    GIBFREI, HOLE, VON,
    
    IDENTIFIER, INT_LITERAL, FLOAT_LITERAL, STRING_LITERAL, BOOL_LITERAL,
    
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LBRACKET, RBRACKET,
    SEMICOLON, COMMA, DOT, COLON,
    
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    ASSIGN,
    NOT,
    LT, GT,
    
    EQ, NEQ,
    LTE, GTE,
    
    DATEIENDE,
    UNBEKANNT
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::GANZ: return "GANZ";
        case TokenType::KOMMA: return "KOMMA";
        case TokenType::WORT: return "WORT";
        case TokenType::JAIN: return "JAIN";
        case TokenType::KISTE: return "KISTE";
        case TokenType::WENN: return "WENN";
        case TokenType::SONST: return "SONST";
        case TokenType::SOLANGE: return "SOLANGE";
        case TokenType::FUER: return "FUER";
        case TokenType::UND: return "UND";
        case TokenType::ODER: return "ODER";
        case TokenType::DRUCKE: return "DRUCKE";
        case TokenType::ZURUECK: return "ZURUECK";
        
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INT_LITERAL: return "INT_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::BOOL_LITERAL: return "BOOL_LITERAL";

        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        
        case TokenType::ASSIGN: return "ASSIGN (=)";
        case TokenType::PLUS: return "PLUS (+)";
        case TokenType::MINUS: return "MINUS (-)";
        case TokenType::MULTIPLY: return "MULTIPLY (*)";
        case TokenType::DIVIDE: return "DIVIDE (/)";
        case TokenType::MODULO: return "MODULO (%)";
        case TokenType::NOT: return "NOT (!)";
        case TokenType::LT: return "LT (<)";
        case TokenType::GT: return "GT (>)";
        case TokenType::EQ: return "EQ (==)";
        case TokenType::NEQ: return "NEQ (!=)";
        case TokenType::LTE: return "LTE (<=)";
        case TokenType::GTE: return "GTE (>=)";
        
        case TokenType::DATEIENDE: return "EOF";
        default: return "UNBEKANNT";
    }
}

class Token {
public:
    TokenType typ;
    std::string wert;
    int zeile;
    int spalte;

    Token(TokenType t, std::string w, int z, int s)
        : typ(t), wert(w), zeile(z), spalte(s) {}

    void print() const {
        std::cout << "Zeile " << zeile << ", Spalte " << spalte
                  << ":\t" << tokenTypeToString(typ)
                  << "\t'" << wert << "'" << std::endl;
    }
};