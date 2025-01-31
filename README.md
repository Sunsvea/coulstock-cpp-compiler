# Simple C++ Compiler

A basic compiler implementation in C++ that supports a subset of C-like syntax. Currently implements: lexical analysis, parsing, and semantic analysis phases.

## Features

- Lexical Analysis (Tokenization)
- Abstract Syntax Tree (AST) Generation
- Semantic Analysis
  - Variable scope tracking
  - Variable initialization checking
  - Basic type checking

## Supported Syntax

Currently supports:
```c
int main() {
    int x = 42;
    if (x > 0) {
        return x * 2;
    }
    return 0;
}
```

### Language Features
- Integer variables
- Basic arithmetic operations (+, -, *, /)
- Comparison operators (>, <, >=, <=, ==)
- If statements
- Return statements
- Block scoping

## Prerequisites

- C++17 compatible compiler (Clang++ recommended)
- Make build system
- CMake (optional)

## Project Structure

```
.
├── include/
│   ├── lexer.hpp
│   ├── parser.hpp
│   ├── semantic_analyzer.hpp
│   └── utils.hpp
├── src/
│   └── main.cpp
├── Makefile
├── .gitignore
└── README.md
```

## Building from Source

1. Clone the repository:
```bash
git clone https://github.com/yourusername/cpp-compiler.git
cd cpp-compiler
```

2. Create build directory:
```bash
mkdir build
```

3. Build using Make:
```bash
make
```

This will create the `compiler` executable in the `build` directory.

## Usage

1. Basic usage:
```bash
./build/compiler
```

The compiler will process the default test program embedded in main.cpp.

2. To modify the input program, edit the `input` string in `src/main.cpp`:
```cpp
std::string input = R"(
    // Your program here
)";
```

## Example Output

The compiler produces detailed output showing each compilation phase:

```
Tokens:
Token: INT | Value: 'int' | Line: 2 | Column: 1
Token: IDENTIFIER | Value: 'main' | Line: 2 | Column: 5
...

Parsing AST:
Function: main
  Block:
    Variable Declaration: x
      Initializer:
        Number: 42
    If Statement:
      Condition:
        Binary Expression:
          Left:
            Identifier: x
          Operator: GREATER
          Right:
            Number: 0
...

Semantic analysis completed successfully!
```

## Error Handling

The compiler provides detailed error messages for various types of errors:

1. Lexical errors (invalid characters, malformed tokens)
2. Syntax errors (invalid program structure)
3. Semantic errors:
   - Use of undeclared variables
   - Use of uninitialized variables
   - Variable redeclaration
   - Scope violations

Example error messages:
```
Semantic Error: Use of undeclared variable 'y'
Semantic Error: Use of uninitialized variable 'x'
Semantic Error: Variable 'x' is already declared in this scope
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Future Improvements

- [ ] Add support for more types (float, bool)
- [ ] Implement function calls
- [ ] Add while loops
- [ ] Add code generation to x86 assembly
- [ ] Implement more operators
- [ ] Add array support
- [ ] Add string support

## Acknowledgments

- Based on modern compiler implementation practices
- Inspired by the LLVM project structure
