#include "generator.h"
#include <iostream>
#include <stdexcept>

std::string gerlang_lese() {    // Liest eine Zeile von der Standardeingabe (Konsole).
    std::string line;
    std::getline(std::cin, line);
    return line;
}

Generator::Generator(std::unique_ptr<Program> program)      // Konstruktor: Nimmt den vom Parser erzeugten AST entgegen.
    : m_program(std::move(program)) {}

std::string Generator::generate() {         // Startet den Generierungsprozess und gibt den fertigen C++-Code als String zurück. Fügt Standard-Includes und Helper-Funktionen hinzu.
    m_output << "#include <iostream>\n";
    m_output << "#include <string>\n\n";
    m_output << "#include <vector>\n\n";

    m_output << "std::string gerlang_lese() {\n";
    m_output << "    std::string line;\n";
    m_output << "    std::getline(std::cin, line);\n";
    m_output << "    return line;\n";
    m_output << "}\n\n";

    m_output << "int gerlang_zu_ganz(std::string s) { try { return std::stoi(s); } catch(...) { return 0; } }\n";   // Wandelt einen String in eine Ganzzahl um (mit Fehlerbehandlung).
    m_output << "float gerlang_zu_komma(std::string s) { try { return std::stof(s); } catch(...) { return 0.0; } }\n";  // Wandelt einen String in eine Kommazahl um (mit Fehlerbehandlung).
    m_output << "std::string gerlang_zu_wort(auto s) { return std::to_string(s); }\n\n";    // Wandelt verschiedene Typen (Zahlen etc.) in einen String um.
    
    visitProgram(m_program.get());
    
    return m_output.str();
}

std::string Generator::mapType(const std::string& gerlangType) {        // Übersetzt GerLang-Typnamen (z.B. "GANZ[]") in C++-Typnamen (z.B. "std::vector<int>").
    if (gerlangType == "GANZ") return "int";
    if (gerlangType == "WORT") return "std::string";
    if (gerlangType == "JAIN") return "bool";
    if (gerlangType == "KOMMA") return "float";
    if (gerlangType == "KISTE") return "std::vector<int>";
    if (gerlangType == "GANZ[]") return "std::vector<int>";
    if (gerlangType == "WORT[]") return "std::vector<std::string>";
    if (gerlangType == "JAIN[]") return "std::vector<bool>";
    if (gerlangType == "KOMMA[]") return "std::vector<float>";
    return "void";
}

std::string mapOperator(const std::string& op) {        // Übersetzt GerLang-Operatoren (z.B. "UND") in C++-Operatoren (z.B. "&&").
    if (op == "UND") return "&&";
    if (op == "ODER") return "||";
    
    return op;
}

void Generator::visit(Statement* stmt) {        // Verteilerfunktion: Leitet den Aufruf an die passende `visit...`-Funktion für den konkreten Statement-Typ weiter.
    if (auto s = dynamic_cast<FunctionDeclaration*>(stmt)) {
        visitFunctionDeclaration(s);
    } else if (auto s = dynamic_cast<BlockStatement*>(stmt)) {
        visitBlockStatement(s);
    } else if (auto s = dynamic_cast<PrintStatement*>(stmt)) {
        visitPrintStatement(s);
    } else if (auto s = dynamic_cast<ReturnStatement*>(stmt)) {
        visitReturnStatement(s);
    } else if (auto s = dynamic_cast<IfStatement*>(stmt)) {
        visitIfStatement(s);
    } else if (auto s = dynamic_cast<VariableDeclaration*>(stmt)) {
        visitVariableDeclaration(s);
    } else if (auto s = dynamic_cast<AssignmentStatement*>(stmt)) {
        visitAssignmentStatement(s);
    } else if (auto s = dynamic_cast<WhileStatement*>(stmt)) {
        visitWhileStatement(s);
    } else if (auto s = dynamic_cast<ForStatement*>(stmt)) {
        visitForStatement(s);
    } else if (auto s = dynamic_cast<ArrayAssignmentStatement*>(stmt)) {
        visitArrayAssignmentStatement(s);
    } else {
        std::cerr << "Generator Fehler: Unbekannter Statement-Typ!" << std::endl;
    }
}

void Generator::visit(Expression* expr) {       // Verteilerfunktion: Leitet den Aufruf an die passende `visit...`-Funktion für den konkreten Expression-Typ weiter.
    if (auto e = dynamic_cast<LiteralExpression*>(expr)) {
        visitLiteralExpression(e);
    } else if (auto e = dynamic_cast<IdentifierExpression*>(expr)) {
        visitIdentifierExpression(e);
    } else if (auto e = dynamic_cast<BinaryExpression*>(expr)) {
        visitBinaryExpression(e);
    } else if (auto e = dynamic_cast<UnaryExpression*>(expr)) {
        visitUnaryExpression(e); 
    } else if (auto e = dynamic_cast<CallExpression*>(expr)) {
        visitCallExpression(e);
    } else if (auto e = dynamic_cast<ArrayLiteralExpression*>(expr)) {
        visitArrayLiteralExpression(e);
    } else if (auto e = dynamic_cast<ArrayAccessExpression*>(expr)) {
        visitArrayAccessExpression(e);
    } else if (auto e = dynamic_cast<LeseExpression*>(expr)) {
        visitLeseExpression(e);
    } else if (auto e = dynamic_cast<TypeConversionExpression*>(expr)) {
        visitTypeConversionExpression(e);
    } else {
         std::cerr << "Generator Fehler: Unbekannter Expression-Typ!" << std::endl;
    }
}

void Generator::visitProgram(Program* p) {      // Besucht den Wurzelknoten des AST und ruft `visit` für alle Top-Level-Statements auf.
    for (auto& stmt : p->statements) {
        if (stmt != nullptr) {
            visit(stmt.get());
        }
    }
}

void Generator::visitFunctionDeclaration(FunctionDeclaration* fd) {     // Generiert den C++-Code für eine Funktionsdefinition (inkl. Parameter).
    if (fd->name == "haupt") {
        m_output << mapType(fd->returnTyp) << " main() \n";
    } else {
        m_output << mapType(fd->returnTyp) << " " << fd->name << "(";
        for (size_t i = 0; i < fd->parameter.size(); ++i) {
            auto& param = fd->parameter[i];
            m_output << mapType(param.first) << " " << param.second;
            if (i < fd->parameter.size() - 1) {
                m_output << ", ";
            }
        }
        m_output << ") \n";
    }
    visit(fd->body.get());
}

void Generator::visitCallExpression(CallExpression* ce) {       // Generiert C++-Code für einen Funktionsaufruf (z.B. `addiere(a, b)`).
    visit(ce->aufrufer.get());
    
    m_output << "(";
    
    for (size_t i = 0; i < ce->argumente.size(); ++i) {
        visit(ce->argumente[i].get());
        if (i < ce->argumente.size() - 1) {
            m_output << ", ";
        }
    }
    
    m_output << ")";
}

void Generator::visitBlockStatement(BlockStatement* bs) {   // Generiert die geschweiften Klammern `{ ... }` und ruft `visit` für alle Statements im Block auf.
    m_output << "{\n";
    for (auto& stmt : bs->statements) {
        if (stmt != nullptr) {
            visit(stmt.get());
        }
    }
    m_output << "}\n";
}

void Generator::visitPrintStatement(PrintStatement* ps) {   // Generiert C++-Code für `std::cout << ... << std::endl;`.
    m_output << "    std::cout << ";
    visit(ps->expression.get());
    m_output << " << std::endl;\n";
}

void Generator::visitReturnStatement(ReturnStatement* rs) {     // Generiert C++-Code für `return ...;`.
    m_output << "    return ";
    visit(rs->wert.get());
    m_output << ";\n";
}

void Generator::visitLiteralExpression(LiteralExpression* le) {     // Generiert den C++-Code für einen Literalwert (Zahl, String, true/false).
    if (le->typName == "GANZ") {
        m_output << std::any_cast<int>(le->wert);
    } else if (le->typName == "WORT") {
        m_output << "\"" << std::any_cast<std::string>(le->wert) << "\"";
    } else if (le->typName == "JAIN") {
        m_output << (std::any_cast<bool>(le->wert) ? "true" : "false");
    } else if (le->typName == "KOMMA") {
        m_output << std::any_cast<float>(le->wert);
    }
}

void Generator::visitIfStatement(IfStatement* is) {     // Generiert C++-Code für `if (...) { ... } else if (...) { ... } else { ... }`.
    m_output << "if (";
    visit(is->condition.get());
    m_output << ") ";
    
    visit(is->thenBranch.get());
    
    if (is->elseBranch != nullptr) {
        m_output << " else ";
        visit(is->elseBranch.get());
    }
    m_output << "\n";
}

void Generator::visitVariableDeclaration(VariableDeclaration* vd) {     // Generiert C++-Code für eine Variablendefinition (z.B. `int zahl = 10;`).
    m_output << "    " << mapType(vd->typName) << " " << vd->name << " = ";
    visit(vd->initializer.get());
    m_output << ";\n";
}

void Generator::visitIdentifierExpression(IdentifierExpression* ie) {       // Generiert den C++-Code für einen Variablennamen.
    m_output << ie->name;
}

void Generator::visitBinaryExpression(BinaryExpression* be) {       // Generiert C++-Code für eine binäre Operation (z.B. `(a + b)` oder `(x < 10)`).
    m_output << "(";
    visit(be->links.get());
    m_output << " " << mapOperator(be->op) << " ";
    visit(be->rechts.get());
    m_output << ")";
}

void Generator::visitUnaryExpression(UnaryExpression* ue) {     // Generiert C++-Code für eine unäre Operation (z.B. `(-5)` oder `(!flag)`).
    std::string op = ue->op;
    if (op == "NICHT") op = "!";
    m_output << "(" << op;
    visit(ue->rechts.get());
    m_output << ")";
}

void Generator::visitAssignmentStatement(AssignmentStatement* as) {     // Generiert C++-Code für eine einfache Zuweisung (z.B. `zahl = 15;`).
    m_output << "    " << as->name << " = ";
    visit(as->wert.get());
    m_output << ";\n";
}

void Generator::visitWhileStatement(WhileStatement* ws) {       // Generiert C++-Code für eine `while (...) { ... }` Schleife.
    m_output << "    while (";
    visit(ws->condition.get());
    m_output << ") ";
    
    visit(ws->body.get());
}

void Generator::visitForStatement(ForStatement* fs) {       // Generiert C++-Code für eine `for (...; ...; ...) { ... }` Schleife.
    m_output << "    for (";
    
    if (fs->initializer != nullptr) {
        if (auto vd = dynamic_cast<VariableDeclaration*>(fs->initializer.get())) {
            m_output << mapType(vd->typName) << " " << vd->name << " = ";
            visit(vd->initializer.get());
        } else if (auto as = dynamic_cast<AssignmentStatement*>(fs->initializer.get())) {
             m_output << as->name << " = ";
             visit(as->wert.get());
        }
    }
    m_output << "; ";
    
    if (fs->condition != nullptr) {
        visit(fs->condition.get());
    }
    m_output << "; ";
    
    if (fs->increment != nullptr) {
        auto as = dynamic_cast<AssignmentStatement*>(fs->increment.get());
        if (as) {
            m_output << as->name << " = ";
            visit(as->wert.get());
        }
    }
    
    m_output << ") ";
    
    visit(fs->body.get());
}

void Generator::visitArrayLiteralExpression(ArrayLiteralExpression* al) {       // Generiert C++-Code für eine Initialisierungsliste (z.B. `{10, 20, 30}`).
    m_output << "{";
    for (size_t i = 0; i < al->elemente.size(); ++i) {
        visit(al->elemente[i].get());
        if (i < al->elemente.size() - 1) {
            m_output << ", ";
        }
    }
    m_output << "}";
}

void Generator::visitArrayAccessExpression(ArrayAccessExpression* aa) {     // Generiert C++-Code für einen Array-Zugriff (z.B. `liste[0]`).
    visit(aa->array.get());
    m_output << "[";
    visit(aa->index.get());
    m_output << "]";
}

void Generator::visitArrayAssignmentStatement(ArrayAssignmentStatement* aas) {      // Generiert C++-Code für eine Zuweisung an ein Array-Element (z.B. `liste[0] = 99;`).
    m_output << "    " << aas->name << "[";
    visit(aas->index.get());
    m_output << "] = ";
    visit(aas->wert.get());
    m_output << ";\n";
}

void Generator::visitLeseExpression(LeseExpression* le) {       // Generiert einen Aufruf der `gerlang_lese()` Helper-Funktion.
    m_output << "gerlang_lese()";
}

void Generator::visitTypeConversionExpression(TypeConversionExpression* tce) {      // Generiert einen Aufruf der passenden `gerlang_zu_...()` Helper-Funktion.
    if (tce->zielTyp == "GANZ") {
        m_output << "gerlang_zu_ganz(";
    } else if (tce->zielTyp == "KOMMA") {
        m_output << "gerlang_zu_komma(";
    } else if (tce->zielTyp == "WORT") {
        m_output << "gerlang_zu_wort(";
    }
    
    visit(tce->expression.get());
    m_output << ")";
}