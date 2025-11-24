#pragma once

#include <sstream>
#include <unordered_map>
#include <cassert>

#include "./parser.hpp"

class Generator {
    public:
        inline explicit Generator(node::Prog* prog) 
        : m_prog(prog) {}

        void generate_expr(const node::Expr* expr) {
            struct ExprVisitor {
                Generator& gen;

                void operator()(const node::ExprIdent* expr_ident) const {
                    std::cout << "Expr Ident\n";

                    if (!gen.m_vars.count(expr_ident->ident.value.value())) {
                        std::cerr << "Undeclared identifier: " << expr_ident->ident.value.value() << "\n";
                        exit(EXIT_FAILURE);
                    }
                    gen.m_output << "    ; ident\n";
                    
                    const Var& var = gen.m_vars[expr_ident->ident.value.value()];
                    std::stringstream offset;
                    offset << "QWORD [rsp + " << (gen.m_stack_size - var.stack_loc - 1) * 8 << "]\n";
                    gen.push(offset.str());
                }

                void operator()(const node::ExprIntLit* expr_intlit) const {
                    std::cout << "Expr Int Lit\n";

                    gen.m_output << "    ; int_lit\n";
                    gen.m_output << "    mov rax, " << expr_intlit->int_lit.value.value() << "\n";
                    gen.push("rax");
                    gen.m_output << "\n";
                }

                void operator()(const node::BinExpr* bin_expr) const {
                    std::cout << "Expr Bin Expr\n";
                    assert(false);
                }
            };

            ExprVisitor visitor { .gen = *this };
            std::visit(visitor, expr->var);
        }

        void generate_stmt(const node::Stmt* stmt) {

            struct StmtVisitor {
                Generator& gen;

                void operator()(const node::StmtExit* stmt_exit) const {
                    std::cout << "Stmt Exit\n";

                    gen.m_output << "    ; exit\n";

                    gen.generate_expr(stmt_exit->expr);

                    gen.m_output << "    mov rax, 60\n";
                    gen.pop("rdi");
                    gen.m_output << "    syscall\n\n";
                }

                void operator()(const node::StmtVar* stmt_var) const {
                    std::cout << "Stmt Var\n";
                    
                    gen.m_output << "    ; var declaration\n";

                    if (gen.m_vars.count(stmt_var->ident.value.value())) {
                        std::cerr << "Identifier already used: " << stmt_var->ident.value.value() << "\n";
                        exit(EXIT_FAILURE);
                    }

                    gen.m_vars.insert({stmt_var->ident.value.value(), Var { .stack_loc = gen.m_stack_size } });
                    gen.generate_expr(stmt_var->expr);
                }
            };

            StmtVisitor visitor { .gen = *this };
            std::visit(visitor, stmt->var);
        }

        [[nodiscard]] inline std::string generate() {
            m_output << "global _start\n\n_start:\n";

            std::cout << m_prog->stmts.size() << " parsed nodes!\n";

            for (const node::Stmt* stmt : m_prog->stmts) {
                std::cout << "Next Stmt...\n";
                generate_stmt(stmt);
            }
            
            m_output << "    mov rax, 60\n";
            m_output << "    mov rdi, 0\n";
            m_output << "    syscall";

            return m_output.str();
        }
    private:
        struct Var {
            size_t stack_loc;
        };

        const node::Prog* m_prog;
        std::stringstream m_output;
        size_t m_stack_size = 0;
        std::unordered_map<std::string, Var> m_vars {};

        void push(const std::string& regist) {
            m_output << "    push " << regist << "\n";
            m_stack_size++;
        }

        void pop(const std::string& regist) {
            m_output << "    pop " << regist << "\n";
            m_stack_size--;
        }
};