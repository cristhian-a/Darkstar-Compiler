#pragma once

#include <vector>
#include <variant>

#include "./arena.hpp"
#include "./tokenizer.hpp"

namespace node {
    struct Expr;

    struct BinExprMultiply { Expr* left; Expr* right; };
    struct BinExprAdd { Expr* left; Expr* right; };
    struct BinExpr { std::variant<BinExprMultiply*, BinExprAdd*> expr; };
    struct ExprIntLit { Token int_lit; };
    struct ExprIdent { Token ident; };
    struct Expr { std::variant<ExprIntLit*, ExprIdent*, BinExpr*> var; };
    struct StmtVar { Token ident; Expr* expr; };
    struct StmtExit { Expr* expr; };
    struct Stmt { std::variant<StmtExit*, StmtVar*> var; };
    struct Prog { std::vector<Stmt*> stmts; };
}

class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens) 
        : m_tokens(std::move(tokens)),
        m_allocator(1024 * 1024 * 4) {  // 4 mb
        }

        std::optional<node::Expr*> parse_expr() {
            if (!peek().has_value()) {
                return {};
            }
            node::Expr* expr = m_allocator.alloc<node::Expr>();
            
            if (peek().value().type == TokenType::int_lit) {
                node::ExprIntLit* intexpr = m_allocator.alloc<node::ExprIntLit>();
                intexpr->int_lit = next();
                expr->var = intexpr;
                return expr;
            } else if (peek().value().type == TokenType::ident) {
                node::ExprIdent* idexpr = m_allocator.alloc<node::ExprIdent>();
                idexpr->ident = next();
                expr->var = idexpr;
                return expr;
            } else {
                return {};
            }
        }

        std::optional<node::Stmt*> parse_stmt() {
            Token current = next();

            if (current.type == TokenType::exit) {
                if (peek().has_value() && peek().value().type == TokenType::open_paren) {
                    next();  // skipping the '('
                } else {
                    std::cerr << "Expected `(`\n";
                    exit(EXIT_FAILURE);
                }

                node::StmtExit* stmt_exit = m_allocator.alloc<node::StmtExit>();
                new (stmt_exit) node::StmtExit;

                if (auto expr_node = parse_expr()) {
                    stmt_exit->expr = expr_node.value();
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

                node::Stmt* stmt = m_allocator.alloc<node::Stmt>();
                new (stmt) node::Stmt;
                stmt->var = stmt_exit;
                return stmt;
            } else if (current.type == TokenType::var) {
                if (peek().has_value() && peek().value().type == TokenType::ident &&
                    peek(1).has_value() && peek(1).value().type == TokenType::equals
                ) {
                    node::StmtVar* stmt_var = m_allocator.alloc<node::StmtVar>();
                    new (stmt_var) node::StmtVar;
                    stmt_var->ident = next();
                    next();  // skipping '='

                    if (const auto expr = parse_expr()) {
                        stmt_var->expr = new (expr.value()) node::Expr;
                    } else {
                        std::cerr << "Invalid expression\n";
                        exit(EXIT_FAILURE);
                    }

                    if (peek().has_value() && peek().value().type == TokenType::semi) {
                        next();  // skipping ';'
                    } else {
                        std::cerr << "Expected `;`\n";
                        exit(EXIT_FAILURE);
                    }

                    node::Stmt* stmt = new(m_allocator.alloc<node::Stmt>()) node::Stmt();
                    new (stmt) node::Stmt();

                    stmt->var = stmt_var;
                    return stmt;
                }
                
            }

            return {};
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
        ArenaAllocator m_allocator;

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