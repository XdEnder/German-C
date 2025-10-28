#include "lexer.h"
#include <cctype>

std::map<std::string, TokenType> Lexer::keywords = {
    {"NIX", TokenType::UNBEKANNT},
    {"GANZ", TokenType::GANZ},
    {"KOMMA", TokenType::KOMMA},
    {"WORT", TokenType::WORT},
    {"JAIN", TokenType::JAIN},
    {"KISTE", TokenType::KISTE},
    {"WENN", TokenType::WENN},
    {"SONST", TokenType::SONST},
    {"SOLANGE", TokenType::SOLANGE},
    {"UND", TokenType::UND},
    {"ODER", TokenType::ODER},
    {"NICHT", TokenType::NICHT},
    {"FUER", TokenType::FUER},
    {"DRUCKE", TokenType::DRUCKE},
    {"LESE", TokenType::LESE},
    {"ZU_GANZ", TokenType::ZU_GANZ},
    {"ZU_KOMMA", TokenType::ZU_KOMMA},
    {"ZU_WORT", TokenType::ZU_WORT},
    {"ZURUECK", TokenType::ZURUECK},
    {"GIBFREI", TokenType::GIBFREI},
    {"HOLE", TokenType::HOLE},
    {"VON", TokenType::VON}
};

std::map<std::string, TokenType> Lexer::literals = {
    {"JA", TokenType::BOOL_LITERAL},
    {"NEIN", TokenType::BOOL_LITERAL}
};

Lexer::Lexer(const std::string& quelltext) : m_quelltext(quelltext) {   // Konstruktor: Initialisiert den Lexer mit dem Quelltext.
    m_aktuellesZeichen = (m_position < m_quelltext.length()) ? m_quelltext[m_position] : '\0';
}

bool Lexer::isAtEnd() {     // Prüft, ob das Ende des Quelltextes erreicht ist.
    return m_position >= m_quelltext.length();
}

void Lexer::vorruecken() {      // Bewegt die Leseposition zum nächsten Zeichen und aktualisiert Zeile/Spalte.
    if (isAtEnd()) {
        m_aktuellesZeichen = '\0';
        return;
    }

    if (m_aktuellesZeichen == '\n') {
        m_zeile++;
        m_spalte = 1;
    } else {
        m_spalte++;
    }

    m_position++;
    m_aktuellesZeichen = (m_position < m_quelltext.length()) ? m_quelltext[m_position] : '\0';
}

char Lexer::peek() {        // Schaut auf das nächste Zeichen, ohne die Position zu verändern.
    int naechstePos = m_position + 1;
    if (naechstePos >= m_quelltext.length()) {
        return '\0';
    }
    return m_quelltext[naechstePos];
}

void Lexer::skipWhitespace() {      // Überspringt alle Leerzeichen, Tabs und Zeilenumbrüche an der aktuellen Position.
    while (!isAtEnd() && isspace(m_aktuellesZeichen)) {
        vorruecken();
    }
}

void Lexer::skipKommentar() {       // Überspringt einen einzeiligen Kommentar (beginnend mit //).
    if (m_aktuellesZeichen == '/' && peek() == '/') {
        while (!isAtEnd() && m_aktuellesZeichen != '\n') {
            vorruecken();
        }
        skipWhitespace();
    }
}

void Lexer::skipMultiLineKommentar() {      // Überspringt einen Blockkommentar (zwischen /* und */).
    vorruecken();
    vorruecken();
    
    while (!isAtEnd()) {
        if (m_aktuellesZeichen == '*' && peek() == '/') {
            break;
        }
        vorruecken();
    }
    
    if (!isAtEnd()) {
        vorruecken();
        vorruecken();
    }
    
    skipWhitespace();
}

Token Lexer::erkenneZahl() {        // Liest eine Ganzzahl (INT_LITERAL) oder Kommazahl (FLOAT_LITERAL).
    std::string wert;
    int startSpalte = m_spalte;
    bool hatPunkt = false;

    while (!isAtEnd() && (isdigit(m_aktuellesZeichen) || m_aktuellesZeichen == '.')) {
        if (m_aktuellesZeichen == '.') {
            if (hatPunkt) break;
            hatPunkt = true;
        }
        wert += m_aktuellesZeichen;
        vorruecken();
    }

    if (hatPunkt) {
        return Token(TokenType::FLOAT_LITERAL, wert, m_zeile, startSpalte);
    }
    return Token(TokenType::INT_LITERAL, wert, m_zeile, startSpalte);
}

Token Lexer::erkenneString() {      // Liest einen String-Literal, der in " oder ' eingeschlossen ist.
    int startSpalte = m_spalte;
    char quote = m_aktuellesZeichen;
    vorruecken();
    
    std::string wert;
    while (!isAtEnd() && m_aktuellesZeichen != quote) {
        wert += m_aktuellesZeichen;
        vorruecken();
    }
    
    if (isAtEnd()) {
        return Token(TokenType::UNBEKANNT, "Fehler: String nicht geschlossen", m_zeile, startSpalte);
    }
    
    vorruecken();
    return Token(TokenType::STRING_LITERAL, wert, m_zeile, startSpalte);
}

Token Lexer::erkenneIdentifierOderKeyword() {       // Liest ein Wort und prüft, ob es ein Keyword (wie WENN) oder ein normaler Identifier (Variablenname) ist.
    std::string wert;
    int startSpalte = m_spalte;
    
    while (!isAtEnd() && (isalnum(m_aktuellesZeichen) || m_aktuellesZeichen == '_')) {
        wert += m_aktuellesZeichen;
        vorruecken();
    }

    if (keywords.count(wert)) {
        return Token(keywords[wert], wert, m_zeile, startSpalte);
    }
    if (literals.count(wert)) {
        return Token(literals[wert], wert, m_zeile, startSpalte);
    }
    
    return Token(TokenType::IDENTIFIER, wert, m_zeile, startSpalte);
}

Token Lexer::naechstesToken() {     // Die Haupt-Scan-Funktion. Identifiziert und gibt das nächste Token aus dem Quelltext zurück, überspringt Leerzeichen und Kommentare.
    while (!isAtEnd()) {
        if (isspace(m_aktuellesZeichen)) {
            skipWhitespace();
            continue;
        }

        if (m_aktuellesZeichen == '/' && peek() == '/') {
            skipKommentar();
            continue;
        }

        if (m_aktuellesZeichen == '/' && peek() == '*') {
            skipMultiLineKommentar();
            continue;
        }

        if (isdigit(m_aktuellesZeichen)) {
            return erkenneZahl();
        }

        if (m_aktuellesZeichen == '"' || m_aktuellesZeichen == '\'') {
            return erkenneString();
        }
        
        if (isalpha(m_aktuellesZeichen) || m_aktuellesZeichen == '_') {
            return erkenneIdentifierOderKeyword();
        }

        int startSpalte = m_spalte;
        char c = m_aktuellesZeichen;
        char p = peek();

        if (c == '=' && p == '=') { vorruecken(); vorruecken(); return Token(TokenType::EQ, "==", m_zeile, startSpalte); }
        if (c == '!' && p == '=') { vorruecken(); vorruecken(); return Token(TokenType::NEQ, "!=", m_zeile, startSpalte); }
        if (c == '<' && p == '=') { vorruecken(); vorruecken(); return Token(TokenType::LTE, "<=", m_zeile, startSpalte); }
        if (c == '>' && p == '=') { vorruecken(); vorruecken(); return Token(TokenType::GTE, ">=", m_zeile, startSpalte); }

        vorruecken();
        switch (c) {
            case '(': return Token(TokenType::LPAREN, "(", m_zeile, startSpalte);
            case ')': return Token(TokenType::RPAREN, ")", m_zeile, startSpalte);
            case '{': return Token(TokenType::LBRACE, "{", m_zeile, startSpalte);
            case '}': return Token(TokenType::RBRACE, "}", m_zeile, startSpalte);
            case '[': return Token(TokenType::LBRACKET, "[", m_zeile, startSpalte);
            case ']': return Token(TokenType::RBRACKET, "]", m_zeile, startSpalte);
            case ';': return Token(TokenType::SEMICOLON, ";", m_zeile, startSpalte);
            case ',': return Token(TokenType::COMMA, ",", m_zeile, startSpalte);
            case '.': return Token(TokenType::DOT, ".", m_zeile, startSpalte);
            case '+': return Token(TokenType::PLUS, "+", m_zeile, startSpalte);
            case '-': return Token(TokenType::MINUS, "-", m_zeile, startSpalte);
            case '*': return Token(TokenType::MULTIPLY, "*", m_zeile, startSpalte);
            case '/': return Token(TokenType::DIVIDE, "/", m_zeile, startSpalte);
            case '%': return Token(TokenType::MODULO, "%", m_zeile, startSpalte);
            case '=': return Token(TokenType::ASSIGN, "=", m_zeile, startSpalte);
            case '<': return Token(TokenType::LT, "<", m_zeile, startSpalte);
            case '>': return Token(TokenType::GT, ">", m_zeile, startSpalte);
            case '!': return Token(TokenType::NOT, "!", m_zeile, startSpalte);
            case ':': return Token(TokenType::COLON, ":", m_zeile, startSpalte);
        }

        return Token(TokenType::UNBEKANNT, std::string(1, c), m_zeile, startSpalte);
    }
    
    return Token(TokenType::DATEIENDE, "EOF", m_zeile, m_spalte);
}

std::vector<Token> Lexer::alleTokens() {      // Ruft `naechstesToken` wiederholt auf, bis der gesamte Quelltext in eine Liste von Tokens umgewandelt wurde.
    std::vector<Token> tokens;
    Token t = naechstesToken();
    while (t.typ != TokenType::DATEIENDE) {
        tokens.push_back(t);
        t = naechstesToken();
    }
    tokens.push_back(t);
    return tokens;
}
