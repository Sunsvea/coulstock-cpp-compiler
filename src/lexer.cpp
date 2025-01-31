#include "lexer.hpp"

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"int", TokenType::INT},
    {"float", TokenType::FLOAT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN}};