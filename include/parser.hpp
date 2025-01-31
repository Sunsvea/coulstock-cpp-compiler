#pragma once
#include <memory>
#include <vector>
#include "lexer.hpp"

// Forward declarations
class Expression;
class Statement;

// AST Node types
enum class NodeType
{
    // Expressions
    BinaryExpr,
    NumberExpr,
    IdentifierExpr,

    // Statements
    FunctionDecl,
    VarDecl,
    ReturnStmt,
    IfStmt,
    BlockStmt
};

// Base AST Node
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual NodeType getType() const = 0;
};

// Expression base class
class Expression : public ASTNode
{
public:
    virtual ~Expression() = default;
};

// Binary Expression (e.g., a + b, x * y)
class BinaryExpression : public Expression
{
public:
    BinaryExpression(std::unique_ptr<Expression> left, TokenType op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    NodeType getType() const override { return NodeType::BinaryExpr; }

    std::unique_ptr<Expression> left;
    TokenType op;
    std::unique_ptr<Expression> right;
};

// Number literal
class NumberExpression : public Expression
{
public:
    NumberExpression(double value) : value(value) {}

    NodeType getType() const override { return NodeType::NumberExpr; }

    double value;
};

// Identifier (variable reference)
class IdentifierExpression : public Expression
{
public:
    IdentifierExpression(std::string name) : name(std::move(name)) {}

    NodeType getType() const override { return NodeType::IdentifierExpr; }

    std::string name;
};

// Statement base class
class Statement : public ASTNode
{
public:
    virtual ~Statement() = default;
};

// Function declaration
class FunctionDeclaration : public Statement
{
public:
    FunctionDeclaration(std::string name, std::vector<std::string> params,
                        std::unique_ptr<Statement> body)
        : name(std::move(name)), parameters(std::move(params)), body(std::move(body)) {}

    NodeType getType() const override { return NodeType::FunctionDecl; }

    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<Statement> body;
};

// Variable declaration
class VariableDeclaration : public Statement
{
public:
    VariableDeclaration(std::string name, std::unique_ptr<Expression> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    NodeType getType() const override { return NodeType::VarDecl; }

    std::string name;
    std::unique_ptr<Expression> initializer;
};

// Return statement
class ReturnStatement : public Statement
{
public:
    ReturnStatement(std::unique_ptr<Expression> value)
        : value(std::move(value)) {}

    NodeType getType() const override { return NodeType::ReturnStmt; }

    std::unique_ptr<Expression> value;
};

// If statement
class IfStatement : public Statement
{
public:
    IfStatement(std::unique_ptr<Expression> condition,
                std::unique_ptr<Statement> thenBranch,
                std::unique_ptr<Statement> elseBranch = nullptr)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    NodeType getType() const override { return NodeType::IfStmt; }

    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
};

// Block statement (sequence of statements)
class BlockStatement : public Statement
{
public:
    BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}

    NodeType getType() const override { return NodeType::BlockStmt; }

    std::vector<std::unique_ptr<Statement>> statements;
};

// Parser class
class Parser
{
public:
    Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

    std::unique_ptr<Statement> parseFunction()
    {
        consume(TokenType::INT, "Expected 'int' before function declaration");
        std::string name = consume(TokenType::IDENTIFIER, "Expected function name").value;
        consume(TokenType::LPAREN, "Expected '(' after function name");
        consume(TokenType::RPAREN, "Expected ')' after parameters");

        auto body = parseBlock();
        return std::make_unique<FunctionDeclaration>(name, std::vector<std::string>(), std::move(body));
    }

private:
    std::vector<Token> tokens;
    size_t current;

    Token peek() const
    {
        if (current >= tokens.size())
            return tokens.back(); // EOF token
        return tokens[current];
    }

    Token advance()
    {
        if (current >= tokens.size())
            return tokens.back();
        return tokens[current++];
    }

    bool check(TokenType type) const
    {
        if (isAtEnd())
            return false;
        return peek().type == type;
    }

    bool isAtEnd() const
    {
        return peek().type == TokenType::EOF_TOKEN;
    }

    Token consume(TokenType type, const std::string &message)
    {
        if (check(type))
            return advance();
        throw std::runtime_error(message + " at line " +
                                 std::to_string(peek().line) + ", column " +
                                 std::to_string(peek().column));
    }

    bool match(TokenType type)
    {
        if (check(type))
        {
            advance();
            return true;
        }
        return false;
    }

    std::unique_ptr<BlockStatement> parseBlock()
    {
        consume(TokenType::LBRACE, "Expected '{' before block");
        std::vector<std::unique_ptr<Statement>> statements;

        while (!match(TokenType::RBRACE) && !isAtEnd())
        {
            statements.push_back(parseStatement());
        }

        return std::make_unique<BlockStatement>(std::move(statements));
    }

    std::unique_ptr<Statement> parseStatement()
    {
        if (match(TokenType::RETURN))
        {
            auto expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expected ';' after return statement");
            return std::make_unique<ReturnStatement>(std::move(expr));
        }

        if (match(TokenType::IF))
        {
            consume(TokenType::LPAREN, "Expected '(' after 'if'");
            auto condition = parseExpression();
            consume(TokenType::RPAREN, "Expected ')' after if condition");
            auto thenBranch = parseStatement();

            std::unique_ptr<Statement> elseBranch = nullptr;
            if (match(TokenType::ELSE))
            {
                elseBranch = parseStatement();
            }

            return std::make_unique<IfStatement>(
                std::move(condition),
                std::move(thenBranch),
                std::move(elseBranch));
        }

        if (match(TokenType::INT))
        {
            std::string name = consume(TokenType::IDENTIFIER, "Expected variable name").value;
            consume(TokenType::ASSIGN, "Expected '=' after variable name");
            auto initializer = parseExpression();
            consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
            return std::make_unique<VariableDeclaration>(name, std::move(initializer));
        }

        throw std::runtime_error("Unexpected token in statement");
    }

    std::unique_ptr<Expression> parseExpression()
    {
        return parseComparison();
    }

    std::unique_ptr<Expression> parseComparison()
    {
        auto expr = parseTerm();

        while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) ||
               match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
               match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL))
        {
            auto op = tokens[current - 1].type;
            auto right = parseTerm();
            expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> parseTerm()
    {
        auto expr = parseFactor();

        while (match(TokenType::PLUS) || match(TokenType::MINUS))
        {
            auto op = tokens[current - 1].type;
            auto right = parseFactor();
            expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> parseFactor()
    {
        auto expr = parsePrimary();

        while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE))
        {
            auto op = tokens[current - 1].type;
            auto right = parsePrimary();
            expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> parsePrimary()
    {
        if (match(TokenType::NUMBER))
        {
            return std::make_unique<NumberExpression>(
                std::stod(tokens[current - 1].value));
        }

        if (match(TokenType::IDENTIFIER))
        {
            return std::make_unique<IdentifierExpression>(
                tokens[current - 1].value);
        }

        if (match(TokenType::LPAREN))
        {
            auto expr = parseExpression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }

        throw std::runtime_error("Expected expression");
    }
};