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
                } else if (current == '/') {
                    // checks for comments and ignores them
                    if (peek().has_value() && peek().value() == '/') {
                        do {
                            next();
                        } while (peek().has_value() && peek().value() != '\n');

                        continue;
                    }
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
                    tokens.push_back({.type = TokenType::open_paren});
                } else if (current == ')') {
                    tokens.push_back({.type = TokenType::close_paren});
                } else if (current == '=') {
                    tokens.push_back({.type = TokenType::equals});
                } else if (current == ';') {
                    tokens.push_back({.type = TokenType::semi, .value = {}});
                    continue;
                } else {
                    std::cerr << "Error!\n";
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