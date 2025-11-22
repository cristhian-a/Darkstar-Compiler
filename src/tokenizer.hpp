#pragma once

#include <iostream>
#include <vector>
#include <optional>

enum class TokenType {
    exit,
    int_lit,
    semi
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
                char current = pop();

                if (std::isspace(current)) {
                    continue;
                } else if (std::isalpha(current)) {
                    buffer.push_back(current);

                    while (peek().has_value() && std::isalnum(peek().value())) {
                        buffer.push_back(pop());
                    }
                    
                    if (buffer == "exit") {
                        tokens.push_back({.type = TokenType::exit, .value = {}});
                        buffer.clear();
                        continue;
                    } else {
                        std::cerr << "Error during keyWord tokenization!\n";
                        std::cerr << buffer << "\n";
                        exit(EXIT_FAILURE);
                    }
                } else if (std::isdigit(current)) {
                    buffer.push_back(current);
                    while (peek().has_value() && std::isdigit(peek().value())) {
                        buffer.push_back(pop());
                    }
                    
                    tokens.push_back({.type = TokenType::int_lit, .value = buffer});
                    buffer.clear();
                    continue;
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
        [[nodiscard]] inline std::optional<char> peek(int ahead = 1) const {
            if (m_index < m_src.length()) {
                return m_src[m_index];
            }

            return {};
        }
        inline char pop() { return m_src[m_index++]; }
};