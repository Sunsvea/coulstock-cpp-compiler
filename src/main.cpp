#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

// Helper function to print the AST
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
    case NodeType::IfStmt:
    {
        auto *ifStmt = static_cast<const IfStatement *>(node);
        std::cout << indentation << "If Statement:" << std::endl;
        std::cout << indentation << "  Condition:" << std::endl;
        printAST(ifStmt->condition.get(), indent + 2);
        std::cout << indentation << "  Then:" << std::endl;
        printAST(ifStmt->thenBranch.get(), indent + 2);
        if (ifStmt->elseBranch)
        {
            std::cout << indentation << "  Else:" << std::endl;
            printAST(ifStmt->elseBranch.get(), indent + 2);
        }
        break;
    }
    case NodeType::VarDecl:
    {
        auto *var = static_cast<const VariableDeclaration *>(node);
        std::cout << indentation << "Variable Declaration: " << var->name << std::endl;
        std::cout << indentation << "  Initializer:" << std::endl;
        printAST(var->initializer.get(), indent + 2);
        break;
    }
    case NodeType::BinaryExpr:
    {
        auto *binary = static_cast<const BinaryExpression *>(node);
        std::cout << indentation << "Binary Expression:" << std::endl;
        std::cout << indentation << "  Left:" << std::endl;
        printAST(binary->left.get(), indent + 2);
        std::cout << indentation << "  Operator: " << tokenTypeToString(binary->op) << std::endl;
        std::cout << indentation << "  Right:" << std::endl;
        printAST(binary->right.get(), indent + 2);
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
};