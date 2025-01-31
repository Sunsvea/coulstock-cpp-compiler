#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include "parser.hpp"

class SemanticError : public std::runtime_error
{
public:
    explicit SemanticError(const std::string &message)
        : std::runtime_error(message) {}
};

class Scope
{
public:
    std::unordered_map<std::string, bool> variables; // variable name -> is initialized
    std::shared_ptr<Scope> parent;

    explicit Scope(std::shared_ptr<Scope> parent = nullptr) : parent(parent) {}

    bool isDeclared(const std::string &name) const
    {
        if (variables.find(name) != variables.end())
        {
            return true;
        }
        return parent ? parent->isDeclared(name) : false;
    }

    bool isInitialized(const std::string &name) const
    {
        auto it = variables.find(name);
        if (it != variables.end())
        {
            return it->second;
        }
        return parent ? parent->isInitialized(name) : false;
    }

    void declare(const std::string &name)
    {
        if (variables.find(name) != variables.end())
        {
            throw SemanticError("Variable '" + name + "' is already declared in this scope");
        }
        variables[name] = false;
    }

    void initialize(const std::string &name)
    {
        auto it = variables.find(name);
        if (it != variables.end())
        {
            it->second = true;
            return;
        }
        if (parent)
        {
            parent->initialize(name);
        }
    }
};

class SemanticAnalyzer
{
private:
    std::shared_ptr<Scope> currentScope;

    void analyzeExpression(const Expression *expr)
    {
        switch (expr->getType())
        {
        case NodeType::BinaryExpr:
        {
            auto *binary = static_cast<const BinaryExpression *>(expr);
            analyzeExpression(binary->left.get());
            analyzeExpression(binary->right.get());
            break;
        }
        case NodeType::IdentifierExpr:
        {
            auto *id = static_cast<const IdentifierExpression *>(expr);
            if (!currentScope->isDeclared(id->name))
            {
                throw SemanticError("Use of undeclared variable '" + id->name + "'");
            }
            if (!currentScope->isInitialized(id->name))
            {
                throw SemanticError("Use of uninitialized variable '" + id->name + "'");
            }
            break;
        }
        case NodeType::NumberExpr:
            // Numbers are always valid
            break;
        default:
            throw SemanticError("Unknown expression type in semantic analysis");
        }
    }

    void analyzeStatement(const Statement *stmt)
    {
        switch (stmt->getType())
        {
        case NodeType::VarDecl:
        {
            auto *varDecl = static_cast<const VariableDeclaration *>(stmt);
            currentScope->declare(varDecl->name);
            if (varDecl->initializer)
            {
                analyzeExpression(varDecl->initializer.get());
                currentScope->initialize(varDecl->name);
            }
            break;
        }
        case NodeType::IfStmt:
        {
            auto *ifStmt = static_cast<const IfStatement *>(stmt);
            analyzeExpression(ifStmt->condition.get());

            auto blockScope = std::make_shared<Scope>(currentScope);
            auto prevScope = currentScope;
            currentScope = blockScope;

            analyzeStatement(ifStmt->thenBranch.get());
            if (ifStmt->elseBranch)
            {
                analyzeStatement(ifStmt->elseBranch.get());
            }

            currentScope = prevScope;
            break;
        }
        case NodeType::BlockStmt:
        {
            auto *block = static_cast<const BlockStatement *>(stmt);
            auto blockScope = std::make_shared<Scope>(currentScope);
            auto prevScope = currentScope;
            currentScope = blockScope;

            for (const auto &s : block->statements)
            {
                analyzeStatement(s.get());
            }

            currentScope = prevScope;
            break;
        }
        case NodeType::ReturnStmt:
        {
            auto *ret = static_cast<const ReturnStatement *>(stmt);
            analyzeExpression(ret->value.get());
            break;
        }
        default:
            throw SemanticError("Unknown statement type in semantic analysis");
        }
    }

public:
    SemanticAnalyzer() : currentScope(std::make_shared<Scope>()) {}

    void analyze(const Statement *root)
    {
        if (root->getType() == NodeType::FunctionDecl)
        {
            auto *func = static_cast<const FunctionDeclaration *>(root);

            // Create new scope for function
            auto functionScope = std::make_shared<Scope>(currentScope);
            auto prevScope = currentScope;
            currentScope = functionScope;

            // Add parameters to scope
            for (const auto &param : func->parameters)
            {
                currentScope->declare(param);
                currentScope->initialize(param);
            }

            // Analyze function body
            analyzeStatement(func->body.get());

            currentScope = prevScope;
        }
        else
        {
            analyzeStatement(root);
        }
    }
};