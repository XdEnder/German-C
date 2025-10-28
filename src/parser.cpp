#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

Token& Parser::peek() { return m_tokens[m_current]; }
Token& Parser::previous() { return m_tokens[m_current - 1]; }
bool Parser::isAtEnd() { return peek().typ == TokenType::DATEIENDE; }

Token& Parser::advance() {
    if (!isAtEnd()) m_current++;
    return previous();
}

bool Parser::check(TokenType typ) {
    if (isAtEnd()) return false;
    return peek().typ == typ;
}

bool Parser::match(std::vector<TokenType> typen) {
    for (TokenType t : typen) {
        if (check(t)) {
            advance();
            return true;
        }
    }
    return false;
}

Token& Parser::consume(TokenType typ, const std::string& errorMsg) {
    if (check(typ)) return advance();
    throw ParseError("Zeile " + std::to_string(peek().zeile) + ": " + errorMsg + 
                     ". Erwartet: " + tokenTypeToString(typ) + 
                     ", aber gefunden: " + tokenTypeToString(peek().typ));
}

std::unique_ptr<Program> Parser::parse() {  // Startet den Parsing-Vorgang für das gesamte Programm und gibt den fertigen AST zurück.
    auto program = std::make_unique<Program>();
    while (!isAtEnd()) {
        program->statements.push_back(declaration());
    }
    return program;
}

std::unique_ptr<Statement> Parser::declaration() {      // Parst eine Deklaration auf oberster Ebene (Funktion oder globale Variable - letzteres noch nicht implementiert). Entscheidet anhand der Tokens, was folgt.
    try {
        if (check(TokenType::GANZ) || check(TokenType::WORT) || check(TokenType::JAIN) || check(TokenType::KISTE) || check(TokenType::KOMMA)) {
            if (m_current + 2 < m_tokens.size() && 
                m_tokens[m_current + 1].typ == TokenType::IDENTIFIER && 
                m_tokens[m_current + 2].typ == TokenType::LPAREN) 
            {
                return functionDeclaration();
            } else {
                return variableDeclaration();
            }
        }
        
        throw ParseError("Zeile " + std::to_string(peek().zeile) + 
                         ": Erwarte eine Deklaration (z.B. GANZ ...)");

    } catch (ParseError& e) {
        std::cerr << "Parser Fehler: " << e.what() << std::endl;
        advance(); 
        return nullptr; 
    }
}

std::unique_ptr<Statement> Parser::statement() {    // Parst eine einzelne Anweisung innerhalb eines Blocks (z.B. DRUCKE, WENN, Zuweisung, Variablendeklaration).
    if (match({TokenType::DRUCKE})) {
        return printStatement();
    }
    if (match({TokenType::ZURUECK})) {
        return returnStatement();
    }
    if (match({TokenType::WENN})) {
        return ifStatement();
    }
    if (match({TokenType::SOLANGE})) {
        return whileStatement();
    }
    if (match({TokenType::FUER})) {
        return forStatement();
    }
    
    if (check(TokenType::GANZ) || check(TokenType::WORT) || check(TokenType::JAIN) || check(TokenType::KISTE) || check(TokenType::KOMMA)) {
        return variableDeclaration();
    }

    if (check(TokenType::IDENTIFIER) && m_tokens[m_current + 1].typ == TokenType::LBRACKET) {
        return arrayAssignmentStatement(); 
    }
    
    if (check(TokenType::IDENTIFIER) && m_tokens[m_current + 1].typ == TokenType::ASSIGN) {
        return assignmentStatement();
    }
    
    throw ParseError("Zeile " + std::to_string(peek().zeile) + ": Unerwartetes Token: " + peek().wert);
}

std::unique_ptr<Statement> Parser::functionDeclaration() {  // Parst eine komplette Funktionsdefinition (Typ, Name, Parameter, Körper).
    auto func = std::make_unique<FunctionDeclaration>();
    
    func->returnTyp = parseType();
    func->name = consume(TokenType::IDENTIFIER, "Erwarte Funktionsnamen").wert;
    consume(TokenType::LPAREN, "Erwarte '(' nach Funktionsnamen");

    if (!check(TokenType::RPAREN)) {
        do {
            std::string paramTypName = parseType();
            Token name = consume(TokenType::IDENTIFIER, "Erwarte Parameternamen");
            func->parameter.push_back({paramTypName, name.wert});
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::RPAREN, "Erwarte ')' nach Parametern");
    consume(TokenType::LBRACE, "Erwarte '{' vor Funktionskörper");
    func->body = blockStatement();
    
    return func;
}

std::unique_ptr<Statement> Parser::variableDeclaration() {  // Parst eine Variablendeklaration (Typ, Name, Initialisierungswert).
    auto decl = std::make_unique<VariableDeclaration>();
    decl->typName = parseType();
    decl->name = consume(TokenType::IDENTIFIER, "Erwarte Variablennamen").wert;
    consume(TokenType::ASSIGN, "Erwarte '=' nach Variablennamen");
    decl->initializer = expression();
    consume(TokenType::SEMICOLON, "Erwarte ';' nach Variablendeklaration");
    return decl;
}

std::unique_ptr<Statement> Parser::assignmentStatement() {  // Parst eine einfache Zuweisung an eine Variable (z.B. name = wert;).
    auto stmt = std::make_unique<AssignmentStatement>();
    stmt->name = consume(TokenType::IDENTIFIER, "Erwarte Variablennamen für Zuweisung").wert;
    consume(TokenType::ASSIGN, "Erwarte '=' bei Zuweisung");
    stmt->wert = expression();
    consume(TokenType::SEMICOLON, "Erwarte ';' nach Zuweisung");
    return stmt;
}

std::unique_ptr<BlockStatement> Parser::blockStatement() {  // Parst einen Codeblock, der von '{' und '}' umschlossen ist und mehrere Statements enthalten kann.
    auto block = std::make_unique<BlockStatement>();
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        block->statements.push_back(statement());
    }
    consume(TokenType::RBRACE, "Erwarte '}' am Ende des Blocks");
    return block;
}

std::unique_ptr<Statement> Parser::printStatement() {   // Parst eine DRUCKE(...) Anweisung.
    auto stmt = std::make_unique<PrintStatement>();
    consume(TokenType::LPAREN, "Erwarte '(' nach DRUCKE");
    stmt->expression = expression();
    consume(TokenType::RPAREN, "Erwarte ')' nach DRUCKE-Ausdruck");
    consume(TokenType::SEMICOLON, "Erwarte ';' nach DRUCKE-Anweisung");
    return stmt;
}

std::unique_ptr<ReturnStatement> Parser::returnStatement() {    // Parst eine ZURUECK ...; Anweisung.
    auto stmt = std::make_unique<ReturnStatement>();
    stmt->wert = expression();
    consume(TokenType::SEMICOLON, "Erwarte ';' nach ZURUECK-Anweisung");
    return stmt;
}

std::unique_ptr<Statement> Parser::ifStatement() {      // Parst eine WENN (...) { ... } [SONST WENN (...) { ... }] [SONST { ... }] Struktur.
    auto stmt = std::make_unique<IfStatement>();
    consume(TokenType::LPAREN, "Erwarte '(' nach WENN");
    stmt->condition = expression();
    consume(TokenType::RPAREN, "Erwarte ')' nach WENN-Bedingung");
    consume(TokenType::LBRACE, "Erwarte '{' nach WENN-Bedingung");
    stmt->thenBranch = blockStatement();
    
    if (match({TokenType::SONST})) {
        if (match({TokenType::WENN})) {
            stmt->elseBranch = ifStatement();
        } else {
            consume(TokenType::LBRACE, "Erwarte '{' nach SONST");
            stmt->elseBranch = blockStatement();
        }
    }
    return stmt;
}

std::unique_ptr<Statement> Parser::whileStatement() {   // Parst eine SOLANGE (...) { ... } Schleife.
    auto stmt = std::make_unique<WhileStatement>();
    consume(TokenType::LPAREN, "Erwarte '(' nach SOLANGE");
    stmt->condition = expression();
    consume(TokenType::RPAREN, "Erwarte ')' nach SOLANGE-Bedingung");
    consume(TokenType::LBRACE, "Erwarte '{' nach SOLANGE-Bedingung");
    stmt->body = blockStatement();
    return stmt;
}

std::unique_ptr<Statement> Parser::forStatement() {     // Parst eine FUER (...; ...; ...) { ... } Schleife.
    auto stmt = std::make_unique<ForStatement>();
    consume(TokenType::LPAREN, "Erwarte '(' nach FUER");
    
    if (match({TokenType::SEMICOLON})) {
        stmt->initializer = nullptr;
    } else if (check(TokenType::GANZ) || check(TokenType::WORT) || check(TokenType::JAIN) || check(TokenType::KISTE)) {
        stmt->initializer = variableDeclaration();
    } else {
        stmt->initializer = assignmentStatement();
    }

    if (!check(TokenType::SEMICOLON)) {
        stmt->condition = expression();
    }
    consume(TokenType::SEMICOLON, "Erwarte ';' nach FUER-Bedingung");
    
    if (!check(TokenType::RPAREN)) {
        auto assign = std::make_unique<AssignmentStatement>();
        assign->name = consume(TokenType::IDENTIFIER, "Erwarte Variablennamen für Inkrement").wert;
        consume(TokenType::ASSIGN, "Erwarte '=' bei Inkrement-Zuweisung");
        assign->wert = expression();
        stmt->increment = std::move(assign);
    }
    consume(TokenType::RPAREN, "Erwarte ')' nach FUER-Klauseln");
    
    consume(TokenType::LBRACE, "Erwarte '{' nach FUER");
    stmt->body = blockStatement();
    return stmt;
}

std::unique_ptr<Statement> Parser::arrayAssignmentStatement() {     // Parst eine Zuweisung an ein Array-Element (z.B. name[index] = wert;).
    auto stmt = std::make_unique<ArrayAssignmentStatement>();
    
    stmt->name = consume(TokenType::IDENTIFIER, "Erwarte Array-Namen").wert;
    
    consume(TokenType::LBRACKET, "Erwarte '[' nach Array-Namen");
    stmt->index = expression();
    consume(TokenType::RBRACKET, "Erwarte ']' nach Array-Index");
    
    consume(TokenType::ASSIGN, "Erwarte '=' bei Array-Zuweisung");
    stmt->wert = expression();
    consume(TokenType::SEMICOLON, "Erwarte ';' nach Array-Zuweisung");
    
    return stmt;
}

std::unique_ptr<Expression> Parser::expression() { return logicalOr(); }    // Einstiegspunkt für das Parsen von Ausdrücken. Ruft die niedrigste Prioritätsstufe auf (logisches ODER).

std::unique_ptr<Expression> Parser::logicalOr() {       // Parst logische ODER (||) Ausdrücke.
    std::unique_ptr<Expression> expr = logicalAnd();
    while (match({TokenType::ODER})) {
        Token op = previous();
        std::unique_ptr<Expression> rechts = logicalAnd();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.wert, std::move(rechts));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {      // Parst logische UND (&&) Ausdrücke.
    std::unique_ptr<Expression> expr = equality();
    while (match({TokenType::UND})) {
        Token op = previous();
        std::unique_ptr<Expression> rechts = equality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.wert, std::move(rechts));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {        // Parst Gleichheits- und Ungleichheitsvergleiche (==, !=).
    std::unique_ptr<Expression> expr = comparison();
    while (match({TokenType::EQ, TokenType::NEQ})) {
        Token op = previous();
        std::unique_ptr<Expression> rechts = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.wert, std::move(rechts));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {      // Parst Vergleiche (<, <=, >, >=).
    std::unique_ptr<Expression> expr = term();
    while (match({TokenType::LT, TokenType::LTE, TokenType::GT, TokenType::GTE})) {
        Token op = previous();
        std::unique_ptr<Expression> rechts = term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.wert, std::move(rechts));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::term() {        // Parst Addition (+) und Subtraktion (-).
    std::unique_ptr<Expression> expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expression> rechts = factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.wert, std::move(rechts));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {      // Parst Multiplikation (*), Division (/) und Modulo (%).
    std::unique_ptr<Expression> expr = unary();
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO})) {
        Token op = previous();
        std::unique_ptr<Expression> rechts = unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.wert, std::move(rechts));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {       // Parst unäre Operatoren am Anfang eines Ausdrucks (z.B. -5 oder NICHT wahr).
    if (match({TokenType::MINUS, TokenType::NOT, TokenType::NICHT})) {
        Token op = previous();
        std::unique_ptr<Expression> rechts = unary();
        return std::make_unique<UnaryExpression>(op.wert, std::move(rechts));
    }
    return call();
}

std::unique_ptr<Expression> Parser::call() {        // Parst Funktionsaufrufe (...) und Array-Zugriffe [...] nach einem primären Ausdruck.
    std::unique_ptr<Expression> expr = primary();

    while (true) { 
        if (match({TokenType::LPAREN})) {
            auto callExpr = std::make_unique<CallExpression>();
            callExpr->aufrufer = std::move(expr);
            // Argumente parsen
            if (!check(TokenType::RPAREN)) {
                do {
                    callExpr->argumente.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RPAREN, "Erwarte ')' nach Funktionsargumenten");
            expr = std::move(callExpr);
            
        } else if (match({TokenType::LBRACKET})) {
            auto accessExpr = std::make_unique<ArrayAccessExpression>();
            accessExpr->array = std::move(expr);
            accessExpr->index = expression();
            consume(TokenType::RBRACKET, "Erwarte ']' nach Array-Index");
            expr = std::move(accessExpr);
            
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {     // Parst die "atomaren" Teile eines Ausdrucks: Literale (Zahlen, Strings, JA/NEIN), Variablennamen, geklammerte Ausdrücke, Array-Literale, LESE() und Typumwandlungen.
    if (match({TokenType::INT_LITERAL})) {
        return std::make_unique<LiteralExpression>(std::stoi(previous().wert), "GANZ");
    }
    if (match({TokenType::FLOAT_LITERAL})) {
        return std::make_unique<LiteralExpression>(std::stof(previous().wert), "KOMMA");
    }
    if (match({TokenType::STRING_LITERAL})) {
        return std::make_unique<LiteralExpression>(previous().wert, "WORT");
    }
    if (match({TokenType::BOOL_LITERAL})) {
        bool wert = (previous().wert == "JA");
        return std::make_unique<LiteralExpression>(wert, "JAIN");
    }
    if (match({TokenType::LBRACKET})) {
        auto arr = std::make_unique<ArrayLiteralExpression>();
        if (!check(TokenType::RBRACKET)) {
            do {
                arr->elemente.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RBRACKET, "Erwarte ']' am Ende der Array-Liste");
        return arr;
    }
    if (match({TokenType::LESE})) {
        consume(TokenType::LPAREN, "Erwarte '(' nach LESE");
        consume(TokenType::RPAREN, "Erwarte ')' nach LESE");
        return std::make_unique<LeseExpression>();
    }
    if (match({TokenType::ZU_GANZ, TokenType::ZU_KOMMA, TokenType::ZU_WORT})) {
        Token typ = previous();
        auto conv = std::make_unique<TypeConversionExpression>();
        
        if (typ.typ == TokenType::ZU_GANZ) conv->zielTyp = "GANZ";
        if (typ.typ == TokenType::ZU_KOMMA) conv->zielTyp = "KOMMA";
        if (typ.typ == TokenType::ZU_WORT) conv->zielTyp = "WORT";
        
        consume(TokenType::LPAREN, "Erwarte '(' nach Typumwandlung");
        conv->expression = expression();
        consume(TokenType::RPAREN, "Erwarte ')' nach Typumwandlung");
        
        return conv;
    }
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierExpression>(previous().wert);
    }
    if (match({TokenType::LPAREN})) {
        std::unique_ptr<Expression> expr = expression();
        consume(TokenType::RPAREN, "Erwarte ')' nach Ausdruck in Klammern");
        return expr;
    }

    throw ParseError("Zeile " + std::to_string(peek().zeile) + 
                     ": Erwarte Ausdruck (Zahl, String, Variable, '[', '(')");
}

std::string Parser::parseType() {       // Liest einen Typnamen (z.B. "GANZ" oder "WORT[]") aus dem Token-Stream.
    if (!match({TokenType::GANZ, TokenType::WORT, TokenType::JAIN, TokenType::KISTE, TokenType::KOMMA})) {
        throw ParseError("Zeile " + std::to_string(peek().zeile) + 
                         ": Erwarte einen Typnamen (GANZ, WORT, KOMMA, KISTE, etc.)");
    }
    Token typ = previous();
    std::string typName = typ.wert;
    
    if (match({TokenType::LBRACKET})) {
        consume(TokenType::RBRACKET, "Erwarte ']' nach '[' in Array-Typ");
        typName += "[]";
    }
    return typName;
}