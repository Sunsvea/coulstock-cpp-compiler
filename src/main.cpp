#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"

// Helper function to print the AST (you can enhance this later)
void printAST(const ASTNode *node, int indent = 0)
{
    std::string indentation(indent * 2, ' ');

    switch (node->getType())
    {
    case NodeType::FunctionDecl:
    {
        auto *func = static_cast<const FunctionDeclaration *>(node);
        std::cout << indentation << "Function: " << func->name << std::endl;
        printAST(func->body.get(), indent + 1);
        break;
    }
    case NodeType::BlockStmt:
    {
        auto *block = static_cast<const BlockStatement *>(node);
        std::cout << indentation << "Block:" << std::endl;
        for (const auto &stmt : block->statements)
        {
            printAST(stmt.get(), indent + 1);
        }
        break;
    }
    case NodeType::ReturnStmt:
    {
        auto *ret = static_cast<const ReturnStatement *>(node);
        std::cout << indentation << "Return:" << std::endl;
        printAST(ret->value.get(), indent + 1);
        break;
    }
    case NodeType::BinaryExpr:
    {
        auto *binary = static_cast<const BinaryExpression *>(node);
        std::cout << indentation << "Binary:" << std::endl;
        printAST(binary->left.get(), indent + 1);
        std::cout << indentation << "  Operator: ";
        switch (binary->op)
        {
        case TokenType::PLUS:
            std::cout << "+";
            break;
        case TokenType::MINUS:
            std::cout << "-";
            break;
        case TokenType::MULTIPLY:
            std::cout << "*";
            break;
        case TokenType::DIVIDE:
            std::cout << "/";
            break;
        default:
            std::cout << "unknown";
        }
        std::cout << std::endl;
        printAST(binary->right.get(), indent + 1);
        break;
    }
    case NodeType::NumberExpr:
    {
        auto *num = static_cast<const NumberExpression *>(node);
        std::cout << indentation << "Number: " << num->value << std::endl;
        break;
    }
    case NodeType::IdentifierExpr:
    {
        auto *id = static_cast<const IdentifierExpression *>(node);
        std::cout << indentation << "Identifier: " << id->name << std::endl;
        break;
    }
    default:
        std::cout << indentation << "Unknown node type" << std::endl;
    }
}

int main()
{
    std::string input = R"(
int main() {
    int x = 42;
    if (x > 0) {
        return x * 2;
    }
    return 0;
}
)";

    try
    {
        // Lexical analysis
        Lexer lexer(input);
        auto tokens = lexer.tokenize();

        std::cout << "Tokens:\n";
        for (const auto &token : tokens)
        {
            std::cout << "Token: " << tokenTypeToString(token.type)
                      << " | Value: '" << token.value
                      << "' | Line: " << token.line
                      << " | Column: " << token.column << std::endl;
        }

        // Parsing
        std::cout << "\nParsing AST:\n";
        Parser parser(tokens);
        auto ast = parser.parseFunction();
        printAST(ast.get());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}