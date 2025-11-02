# German-C (gppc) 🇩🇪➡️💻

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Language: C++](https://img.shields.io/badge/Language-C++-00599C.svg)](https://isocpp.org/)

**German C (gppc): A fun experimental project creating a C++ transpiler. Write code using German keywords (`GANZ`, `WENN`, `SOLANGE`) in `.gc` files and compile them into executable C++ programs.**

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
git clone <repository-url>
cd German-C
```

**Compilation**

On Linux / macOS: Simply run `make` in the main project directory:

```bash
make
```

This will compile the `gppc` transpiler and place it in the project's root directory.

**On Windows / Manual Compilation:** This project has been primarily tested on Linux. The `Makefile` might not work directly on Windows without adjustments. You will need a C++ compiler (`g++` via MinGW/MSYS2 is recommended). You can compile manually using:

```bash
g++ src/*.cpp -o gppc -std=c++17 -Wall -g
```

**Running the Compiler (gppc)**

Once `gppc` is compiled, you can use it to transpile and compile your `.gc` files:

**1. Run gppc on your source file:**
```bash
./gppc <your_source_file.gc>
```
For example:
```bash
./gppc examples/01_hallo_welt.gc
```
This command will:

* Parse your `.gc` file.
* Generate intermediate C++ code (`_temp.cpp`).
* Compile the C++ code using `g++` into an executable (e.g., `01_hallo_welt`).
* Clean up the temporary file.

**2. Run the compiled program:**
```bash
./<your_source_file_base_name>
```
For example:
```bash
./01_hallo_welt
```

## 📚 Examples

Check out the `examples/` directory for sample programs demonstrating various language features.

**Example:** `examples/01_hallo_welt.gc`
```bash
/*
 * 01_hallo_welt.gc
 * Das ist ein einfaches "Hallo Welt" Programm.
 */

GANZ haupt() {
    WORT gruss = "Hallo";
    WORT name = "Welt";

    DRUCKE(gruss + ", " + name + "!"); // Ausgabe: Hallo, Welt!

    ZURUECK 0;
}
```

**Compile and Run:**
```bash
make          # If you haven't compiled gppc yet
./gppc examples/01_hallo_welt.gc
./examples/01_hallo_welt
```

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
