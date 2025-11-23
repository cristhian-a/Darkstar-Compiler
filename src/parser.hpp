#pragma once

#include <vector>
#include <variant>

#include "./tokenizer.hpp"

namespace node {
    struct ExprIntLit { Token int_lit; };
    struct ExprIdent { Token ident; };
    struct Expr { std::variant<ExprIntLit, ExprIdent> var; };
    struct StmtVar { Token ident; Expr expr; };
    struct StmtExit { Expr expr; };
    struct Stmt { std::variant<StmtExit, StmtVar> var; };
    struct Prog { std::vector<Stmt> stmts; };
}

class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens) 
            : m_tokens(std::move(tokens)) {}

        std::optional<node::Expr> parse_expr() {
            if (peek().has_value() && peek().value().type == TokenType::int_lit) {
                return node::Expr { .var = node::ExprIntLit{ .int_lit = next() } };
            } else if (peek().has_value() && peek().value().type == TokenType::ident) {
                return node::Expr { .var = node::ExprIdent{ .ident = next() } };
            }
            return {};
        }

        std::optional<node::Stmt> parse_stmt() {
            std::optional<node::Stmt> result_stmt;
            Token current = next();

            if (current.type == TokenType::exit) {
                if (peek().has_value() && peek().value().type == TokenType::open_paren) {
                    next();  // skipping the '('
                } else {
                    std::cerr << "Expected `(`\n";
                    exit(EXIT_FAILURE);
                }

                node::StmtExit stmt_exit;

                if (auto expr_node = parse_expr()) {
                    stmt_exit = node::StmtExit { .expr = expr_node.value() };
                } else {
                    std::cerr << "Invalid expression!\n";
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() && peek().value().type == TokenType::close_paren) {
                    next();  // skipping the ')'
                } else {
                    std::cerr << "Expected `)`\n";
                    exit(EXIT_FAILURE);
                }
                    
                if (peek().has_value() && peek().value().type == TokenType::semi) {
                    next();  // skipping the semicolun
                } else {
                    std::cerr << "Expected `;`\n";
                    exit(EXIT_FAILURE);
                }

                result_stmt = node::Stmt { .var = stmt_exit };
            } else if (current.type == TokenType::var) {
                if (peek().has_value() && peek().value().type == TokenType::ident &&
                    peek(1).has_value() && peek(1).value().type == TokenType::equals
                ) {
                    node::StmtVar stmt_var = node::StmtVar {.ident = next()};
                    next();  // skipping the equal sign
                    if (auto expr = parse_expr()) {
                        stmt_var.expr = expr.value();
                    } else {
                        std::cerr << "Invalid expression\n";
                        exit(EXIT_FAILURE);
                    }

                    if (peek().has_value() && peek().value().type == TokenType::semi) {
                        next();  // skipping the semicolun
                    } else {
                        std::cerr << "Expected `;`\n";
                        exit(EXIT_FAILURE);
                    }

                    result_stmt = node::Stmt { .var = stmt_var };
                }
                
            }

            return result_stmt;
        }

        std::optional<node::Prog> parse_prog() {
            node::Prog prog_node;

            while (peek().has_value()) {
                if (auto stmt = parse_stmt()) {
                    prog_node.stmts.push_back(stmt.value());
                } else {
                    std::cerr << "Invalid statement\n";
                }
            }

            return prog_node;
        }
        
    private:
        const std::vector<Token> m_tokens;
        size_t m_index = 0;
        [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const {
            if (m_index + offset < m_tokens.size()) {
                return m_tokens[m_index + offset];
            }

            return {};
        }
        inline Token next() {
            return m_tokens[m_index++];
        }

};