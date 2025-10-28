# German-C (gppc) 🇩🇪➡️💻

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

**German C (gppc): A fun experimental project creating a C++ transpiler. Write code using German keywords (`GANZ`, `WENN`, `SOLANGE`) in `.gc` files and compile them into executable C++ programs. Inspired by [KibaOfficial/gerlang](https://github.com/KibaOfficial/gerlang).**

---

## ✨ What is German-C?

This project is a **transpiler** written in C++. It takes source code files with a `.gc` extension, which use German keywords and a syntax similar to C/C++/Java, and translates them into standard C++ code. This C++ code is then compiled using a regular C++ compiler (like `g++`) into a native executable.

The goal was to experiment with compiler construction concepts (lexing, parsing, code generation) and create a small, usable programming language with German syntax.

---

## 🚀 Features

German-C currently supports a solid set of features:

* **German Keywords:** Use intuitive German words for programming constructs (e.g., `GANZ`, `WENN`, `SOLANGE`, `FUER`, `DRUCKE`, `LESE`).
* **Basic Data Types:** `GANZ` (int), `KOMMA` (float), `WORT` (string), `JAIN` (bool).
* **Arrays:** Declare (`KISTE` or `GANZ[]`), initialize (`[1, 2, 3]`), read (`liste[0]`), and write (`liste[0] = 99`).
* **Operators:** Arithmetic (`+`, `-`, `*`, `/`, `%`), Comparison (`==`, `!=`, `<`, `>`, `<=`, `>=`), Logical (`UND`, `ODER`, `NICHT`).
* **Control Flow:** `WENN / SONST WENN / SONST` statements, `SOLANGE` loops, `FUER` loops.
* **Functions:** Define your own functions with parameters and return values (`ZURUECK`).
* **I/O:** Print to console (`DRUCKE(...)`) and read user input (`LESE()`).
* **Type Conversions:** Convert between types (`ZU_GANZ()`, `ZU_KOMMA()`, `ZU_WORT()`).
* **Comments:** Single-line (`//`) and multi-line (`/* ... */`).

---

## ⚙️ Getting Started

### Prerequisites

* **A C++17 compliant compiler:** Such as `g++` (recommended) or `clang++`.
* **`make` build tool:** Standard on Linux and macOS. Windows users might need to install it (e.g., via MinGW/MSYS2).

### Cloning the Repository

```bash
git clone [https://github.com/XdEnder/German-C.git](https://github.com/XdEnder/German-C.git)
cd German-C
