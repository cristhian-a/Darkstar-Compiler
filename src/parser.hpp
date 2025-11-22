#pragma once

#include <vector>
#include "./tokenizer.hpp"

namespace node {
    struct Expr {
        Token int_lit;
    };

    struct Exit {
        Expr expr;
    };
}

class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens) 
            : m_tokens(std::move(tokens)) {}
        std::optional<node::Expr> parse_expr() {
            if (peek().has_value() && peek().value().type == TokenType::int_lit) {
                return node::Expr { .int_lit = pop() };
            }
            return {};
        }
        std::optional<node::Exit> parse() {
            std::optional<node::Exit> exit_node;

            while (peek().has_value()) {
                Token current = pop();
                if (current.type == TokenType::exit) {
                    if (auto expr_node = parse_expr()) {
                        exit_node = node::Exit { .expr = expr_node.value() };
                    } else {
                        std::cerr << "Invalid expression!\n";
                        exit(EXIT_FAILURE);
                    }
                    
                    if (!peek().has_value() || peek().value().type != TokenType::semi) {
                        std::cerr << "No semicolun finishing the expression!\n";
                        exit(EXIT_FAILURE);
                    }
                    
                    pop();  // popping the semicolun
                }
                
            }
            
            m_index = 0;
            return exit_node;
        }
    private:
        const std::vector<Token> m_tokens;
        size_t m_index = 0;
        [[nodiscard]] inline std::optional<Token> peek(int ahead = 1) const {
            if (m_index < m_tokens.size()) {
                return m_tokens[m_index];
            }

            return {};
        }
        inline Token pop() {
            return m_tokens[m_index++];
        }

};