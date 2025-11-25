#pragma once

#include <iostream>
#include <vector>
#include <optional>

enum class TokenType {
    exit,
    var,
    ident,
    int_lit,
    semi,
    open_paren,
    close_paren,
    equals
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
    public:
        inline explicit Tokenizer(std::string src) 
        : m_src(std::move(src)) {}

        inline std::vector<Token> tokenize() {
            std::vector<Token> tokens;
            std::string buffer;

            while (peek().has_value()) {
                char current = next();

                if (std::isspace(current)) {
                    continue;
                } else if (current == '/' && peek().has_value() && peek().value() == '/') {
                    // checks for comments and ignores them
                    while (peek().has_value() && peek().value() != '\n') {
                        next();
                    }
                    
                    continue;
                } else if (current == '/' && peek().has_value() && peek().value() == '*') {
                    next(); // consumes '*'
                    
                    // checks multi-line comments and ignores them
                    while (!(peek().has_value() && peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/')) {
                        next();
                    }

                    if (peek().has_value()) next(); // consumes final '*'
                    if (peek().has_value()) next(); // consumes final '/'
                    
                    continue;
                } else if (std::isalpha(current)) {
                    buffer.push_back(current);

                    while (peek().has_value() && std::isalnum(peek().value())) {
                        buffer.push_back(next());
                    }
                    
                    if (buffer == "exit") {
                        tokens.push_back({.type = TokenType::exit, .value = {}});
                    } else if (buffer == "var") {
                        tokens.push_back({.type = TokenType::var, .value = {}});
                    } else {
                        tokens.push_back({.type = TokenType::ident, .value = buffer});
                    }

                    buffer.clear();
                    continue;
                } else if (std::isdigit(current)) {
                    buffer.push_back(current);
                    while (peek().has_value() && std::isdigit(peek().value())) {
                        buffer.push_back(next());
                    }
                    
                    tokens.push_back({.type = TokenType::int_lit, .value = buffer});
                    buffer.clear();
                    continue;
                } else if (current == '(') {
                    tokens.push_back({.type = TokenType::open_paren, .value = {}});
                } else if (current == ')') {
                    tokens.push_back({.type = TokenType::close_paren, .value = {}});
                } else if (current == '=') {
                    tokens.push_back({.type = TokenType::equals, .value = {}});
                } else if (current == ';') {
                    tokens.push_back({.type = TokenType::semi, .value = {}});
                    continue;
                } else {
                    std::cerr << "Error trying to tokenize the following element:\n";
                    std::cerr << current << "\n";
                    exit(EXIT_FAILURE);
                }
                
            }
        
            m_index = 0;
            return tokens;
        }
    private:
        const std::string m_src;
        size_t m_index = 0;
        [[nodiscard]] inline std::optional<char> peek(int offset = 0) const {
            if (m_index + offset < m_src.length()) {
                return m_src[m_index + offset];
            }

            return {};
        }
        inline char next() { return m_src[m_index++]; }
};