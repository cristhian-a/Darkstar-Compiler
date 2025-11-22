#pragma once

#include <sstream>

#include "./parser.hpp"

class Generator {
    public:
        inline explicit Generator(node::Exit exit_node) 
        : m_root(std::move(exit_node)) {}
        
        [[nodiscard]] inline std::string generate() const {
            std::stringstream output;

            output << "global _start\n";
            output << "\n";
            output << "_start:\n";

            output << "    mov rax, 60\n";
            output << "    mov rdi, " << m_root.expr.int_lit.value.value() << "\n";
            output << "    syscall";

            return output.str();
        }
    private:
        const node::Exit m_root;
};