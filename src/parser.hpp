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

// assume node:: types are:
// struct Prog { std::vector<Stmt*> stmts; };
// struct Stmt { std::variant<StmtExit*, StmtVar*> var; };
// struct Expr { std::variant<ExprIntLit*, ExprIdent*, BinExpr*> var; };
// etc.
// and Token, TokenType exist.

class Parser {
public:
    // parser borrows the arena instance (owned by caller or kept alive by caller)
    Parser(std::vector<Token> tokens, ArenaAllocator& allocator)
      : m_tokens(std::move(tokens)), m_allocator(allocator) { }

    // parse_prog now returns pointer allocated in arena
    node::Prog* parse_prog() {
        node::Prog* prog = m_allocator.alloc<node::Prog>(); // constructs vector
        while (peek().has_value()) {
            auto maybe_stmt = parse_stmt();
            if (maybe_stmt) {
                prog->stmts.push_back(maybe_stmt.value());
            } else {
                std::cerr << "Invalid statement\n";
                break;
            }
        }
        return prog;
    }

    // parse a statement, returning arena-allocated Stmt*
    std::optional<node::Stmt*> parse_stmt() {
        if (!peek().has_value()) return {};

        Token current = next();

        if (current.type == TokenType::exit) {
            // expect '(' ... ')' ';'
            if (!(peek().has_value() && peek().value().type == TokenType::open_paren)) {
                std::cerr << "Expected `(`\n";
                return {};
            }
            next(); // consume '('

            node::StmtExit* exitNode = m_allocator.alloc<node::StmtExit>();
            auto maybe_expr = parse_expr();
            if (!maybe_expr) {
                std::cerr << "Invalid expression!\n";
                return {};
            }
            exitNode->expr = maybe_expr.value();

            if (!(peek().has_value() && peek().value().type == TokenType::close_paren)) {
                std::cerr << "Expected `)`\n";
                return {};
            }
            next(); // consume ')'

            if (!(peek().has_value() && peek().value().type == TokenType::semi)) {
                std::cerr << "Expected `;`\n";
                return {};
            }
            next(); // consume ';'

            node::Stmt* s = m_allocator.alloc<node::Stmt>();
            s->var = exitNode;
            return s;
        }
        else if (current.type == TokenType::var) {
            // expect ident '=' expr ';'
            if (!(peek().has_value() && peek().value().type == TokenType::ident)) {
                std::cerr << "Expected identifier after 'var'\n";
                return {};
            }
            // consume identifier
            Token identTok = next();

            if (!(peek().has_value() && peek().value().type == TokenType::equals)) {
                std::cerr << "Expected '=' after identifier\n";
                return {};
            }
            next(); // consume '='

            auto maybe_expr = parse_expr();
            if (!maybe_expr) {
                std::cerr << "Invalid expression\n";
                return {};
            }

            if (!(peek().has_value() && peek().value().type == TokenType::semi)) {
                std::cerr << "Expected `;`\n";
                return {};
            }
            next(); // consume ';'

            node::StmtVar* stmtVar = m_allocator.alloc<node::StmtVar>();
            stmtVar->ident = identTok;
            stmtVar->expr = maybe_expr.value();

            node::Stmt* s = m_allocator.alloc<node::Stmt>();
            s->var = stmtVar;
            return s;
        }

        return {};
    }

    // parse an expression, returns Expr* allocated in arena
    std::optional<node::Expr*> parse_expr() {
        if (!peek().has_value()) return {};

        node::Expr* expr = m_allocator.alloc<node::Expr>(); // construct variant

        Token t = peek().value();
        if (t.type == TokenType::int_lit) {
            node::ExprIntLit* intnode = m_allocator.alloc<node::ExprIntLit>();
            intnode->int_lit = next();
            expr->var = intnode;
            return expr;
        } else if (t.type == TokenType::ident) {
            node::ExprIdent* idnode = m_allocator.alloc<node::ExprIdent>();
            idnode->ident = next();
            expr->var = idnode;
            return expr;
        } else {
            return {};
        }
    }

private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator& m_allocator;

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