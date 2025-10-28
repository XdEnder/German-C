// src/main.cpp

// Standard C++ Bibliotheken für Ein-/Ausgabe, Dateizugriff, etc.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "generator.h"

/*
 * ============================================
 * ===== gppc - Der German C++ Compiler =======
 * ============================================
 * Der Haupteinstiegspunkt des gppc Compilers (Transpilers).
 *
 * Ablauf:
 * 1. Liest den Dateinamen der .gc-Datei aus den Kommandozeilenargumenten.
 * 2. Öffnet und liest die .gc Quelldatei komplett ein.
 * 3. Startet den Lexer (lexer.cpp), um den Quelltext in eine Liste von Tokens zu zerlegen.
 * 4. Startet den Parser (parser.cpp), um aus den Tokens einen Abstract Syntax Tree (AST) zu bauen.
 * 5. Startet den Generator (generator.cpp), um aus dem AST C++-Code zu erzeugen.
 * 6. Schreibt den generierten C++-Code in eine temporäre Datei (_temp.cpp).
 * 7. Ruft den externen C++ Compiler (g++) auf, um die temporäre Datei zu kompilieren und das finale Programm zu erstellen.
 * 8. Löscht die temporäre C++-Datei.
 * 9. Gibt Erfolgs- oder Fehlermeldungen auf der Konsole aus.
 */
int main(int argc, char* argv[]) {
    // 1. Argumente prüfen
    if (argc != 2) {
        std::cerr << "Benutzung: ./gppc <dateiname.gc>" << std::endl;
        return 1;
    }
    
    std::string gcDateiname = argv[1];
    
    // 2. Datei einlesen
    std::ifstream datei(gcDateiname);
    if (!datei.is_open()) {
        std::cerr << "Fehler: Datei '" << gcDateiname << "' konnte nicht geöffnet werden." << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << datei.rdbuf();
    std::string quelltext = buffer.str();
    datei.close();

    // 3. Lexer starten
    std::cout << "-> [gppc] Phase 1: Lexer startet..." << std::endl;
    Lexer lexer(quelltext);
    std::vector<Token> tokens = lexer.alleTokens();

    // 4. Parser starten
    std::cout << "-> [gppc] Phase 2: Parser startet..." << std::endl;
    Parser parser(tokens);
    std::unique_ptr<Program> ast;
    try {
        ast = parser.parse();
    } catch (const ParseError& e) {
        std::cerr << "Parser Fehler: " << e.what() << std::endl;
        return 1; // Bei Syntaxfehler abbrechen
    }

    // 5. Generator starten
    std::cout << "-> [gppc] Phase 3: Generator startet..." << std::endl;
    Generator generator(std::move(ast));
    std::string cppCode = generator.generate();
    
    // 6. Temporäre C++ Datei schreiben
    std::string tempCppDatei = "_temp.cpp";
    std::ofstream tempOut(tempCppDatei);
    tempOut << cppCode;
    tempOut.close();
    
    // Zieldateinamen bestimmen (z.B. "test.gc" -> "test")
    std::string outputName = gcDateiname.substr(0, gcDateiname.find_last_of("."));

    // 7. Externen Compiler (g++) aufrufen
    std::cout << "-> [gppc] Phase 4: Kompiliere zu '" << outputName << "'..." << std::endl;
    std::string compileBefehl = "g++ " + tempCppDatei + " -o " + outputName + " -std=c++17";
    
    int compileStatus = system(compileBefehl.c_str());
    
    // 8. Temporäre Datei löschen
    remove(tempCppDatei.c_str());
    
    // 9. Ergebnis melden
    if (compileStatus == 0) {
        std::cout << "-> [gppc] Erfolg! Programm '" << outputName << "' wurde erstellt." << std::endl;
        std::cout << "-> Führe es aus mit: ./" << outputName << std::endl;
    } else {
        std::cerr << "-> [gppc] Fehler beim Kompilieren mit g++!" << std::endl;
    }
    
    return compileStatus;
}