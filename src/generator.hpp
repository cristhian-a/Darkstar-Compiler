#pragma once

#include <sstream>

#include "./parser.hpp"

class Generator {
    public:
        inline explicit Generator(node::Prog prog) 
        : m_prog(std::move(prog)) {}

        void generate_expr(const node::Expr& expr) {
            struct ExprVisitor {
                Generator* gen;

                void operator()(const node::ExprIdent& expr_ident) const {
                    gen->m_output << "    mov rax, " << expr_ident.ident.value.value() << "\n";
                    gen->m_output << "    push rax\n\n";
                }
                void operator()(const node::ExprIntLit& expr_intlit) {
                    gen->m_output << "    mov rax, " << expr_intlit.int_lit.value.value() << "\n";
                    gen->m_output << "    push rax\n\n";
                }
            };

            ExprVisitor visitor { .gen = this };
            std::visit(visitor, expr.var);
        }

        void generate_stmt(const node::Stmt& stmt) {
            struct StmtVisitor {
                Generator* gen;

                void operator()(const node::StmtExit& stmt_exit) const {
                    gen->generate_expr(stmt_exit.expr);

                    gen->m_output << "    mov rax, 60\n";
                    gen->m_output << "    pop rdi\n";
                    gen->m_output << "    syscall\n\n";
                }
                void operator()(const node::StmtVar& stmt_var) {}
            };

            StmtVisitor visitor { .gen = this };
            std::visit(visitor, stmt.var);
        }

        [[nodiscard]] inline std::string generate() {

            m_output << "global _start\n\n_start:\n";

            for (const node::Stmt& stmt : m_prog.stmts) {
                generate_stmt(stmt);
            }
            
            m_output << "    mov rax, 60\n";
            m_output << "    mov rdi, 0\n";
            m_output << "    syscall";

            return m_output.str();
        }
    private:
        const node::Prog m_prog;
        std::stringstream m_output;
};