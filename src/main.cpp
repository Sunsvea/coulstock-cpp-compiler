#include <iostream>
#include "lexer.hpp"

std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::INT:
        return "INT";
    case TokenType::FLOAT:
        return "FLOAT";
    case TokenType::IF:
        return "IF";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::MULTIPLY:
        return "MULTIPLY";
    case TokenType::DIVIDE:
        return "DIVIDE";
    case TokenType::ASSIGN:
        return "ASSIGN";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::NOT_EQUAL:
        return "NOT_EQUAL";
    case TokenType::LESS:
        return "LESS";
    case TokenType::GREATER:
        return "GREATER";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::LPAREN:
        return "LPAREN";
    case TokenType::RPAREN:
        return "RPAREN";
    case TokenType::LBRACE:
        return "LBRACE";
    case TokenType::RBRACE:
        return "RBRACE";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::EOF_TOKEN:
        return "EOF";
    default:
        return "UNKNOWN";
    }
}

int main()
{
    // Test input
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
        Lexer lexer(input);
        auto tokens = lexer.tokenize();

        for (const auto &token : tokens)
        {
            std::cout << "Token: " << tokenTypeToString(token.type)
                      << " | Value: '" << token.value
                      << "' | Line: " << token.line
                      << " | Column: " << token.column << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}