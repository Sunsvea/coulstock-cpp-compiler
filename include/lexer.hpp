#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <stdexcept>

enum class TokenType
{
    // Keywords
    INT,
    FLOAT,
    IF,
    ELSE,
    WHILE,
    RETURN,

    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS,
    GREATER,

    // Other
    IDENTIFIER,
    NUMBER,
    LPAREN,
    RPAREN, // ( )
    LBRACE,
    RBRACE, // { }
    SEMICOLON,
    EOF_TOKEN,
};

struct Token
{
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, std::string v, int l, int c)
        : type(t), value(std::move(v)), line(l), column(c) {}
};

class Lexer
{
private:
    std::string input;
    size_t position = 0;
    int line = 1;
    int column = 1;

    static const std::unordered_map<std::string, TokenType> keywords;

    char peek() const
    {
        if (position >= input.length())
            return '\0';
        return input[position];
    }

    char advance()
    {
        char current = peek();
        position++;
        column++;
        if (current == '\n')
        {
            line++;
            column = 1;
        }
        return current;
    }

    void skipWhitespace()
    {
        while (std::isspace(peek()))
        {
            advance();
        }
    }

    Token readNumber()
    {
        std::string result;
        int startColumn = column;

        while (std::isdigit(peek()) || peek() == '.')
        {
            result += advance();
        }

        return Token(TokenType::NUMBER, result, line, startColumn);
    }

    Token readIdentifier()
    {
        std::string result;
        int startColumn = column;

        while (std::isalnum(peek()) || peek() == '_')
        {
            result += advance();
        }

        // Check if it's a keyword
        auto it = keywords.find(result);
        if (it != keywords.end())
        {
            return Token(it->second, result, line, startColumn);
        }

        return Token(TokenType::IDENTIFIER, result, line, startColumn);
    }

public:
    explicit Lexer(std::string source) : input(std::move(source)) {}

    Token nextToken()
    {
        skipWhitespace();

        if (position >= input.length())
        {
            return Token(TokenType::EOF_TOKEN, "", line, column);
        }

        char current = peek();
        int currentColumn = column;

        // Handle numbers
        if (std::isdigit(current))
        {
            return readNumber();
        }

        // Handle identifiers and keywords
        if (std::isalpha(current) || current == '_')
        {
            return readIdentifier();
        }

        // Handle operators and other single characters
        advance(); // consume the character

        switch (current)
        {
        case '+':
            return Token(TokenType::PLUS, "+", line, currentColumn);
        case '-':
            return Token(TokenType::MINUS, "-", line, currentColumn);
        case '*':
            return Token(TokenType::MULTIPLY, "*", line, currentColumn);
        case '/':
            return Token(TokenType::DIVIDE, "/", line, currentColumn);
        case '=':
            return Token(TokenType::ASSIGN, "=", line, currentColumn);
        case '(':
            return Token(TokenType::LPAREN, "(", line, currentColumn);
        case ')':
            return Token(TokenType::RPAREN, ")", line, currentColumn);
        case '{':
            return Token(TokenType::LBRACE, "{", line, currentColumn);
        case '}':
            return Token(TokenType::RBRACE, "}", line, currentColumn);
        case ';':
            return Token(TokenType::SEMICOLON, ";", line, currentColumn);
        default:
            throw std::runtime_error("Unexpected character: " + std::string(1, current));
        }
    }

    std::vector<Token> tokenize()
    {
        std::vector<Token> tokens;
        while (true)
        {
            Token token = nextToken();
            tokens.push_back(token);
            if (token.type == TokenType::EOF_TOKEN)
                break;
        }
        return tokens;
    }
};