#include <iostream>
#include <fstream>
#include <optional>
#include <vector>

#include "./generator.hpp"
#include "./parser.hpp"
#include "./tokenizer.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong arguments! You should use: " << std::endl;
        std::cerr << "darkstar <input.ds>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contentStream;
        std::fstream input(argv[1], std::ios::in);
        contentStream << input.rdbuf();
        contents = contentStream.str();
    }

    std::vector<Token> tokens = Tokenizer(std::move(contents)).tokenize();
    std::optional<node::Prog> prog = Parser(std::move(tokens)).parse_prog();

    if (!prog.has_value()) {
        std::cerr << "No valid program statements!\n";
        return EXIT_FAILURE;
    }
    

    std::string asmOutput = Generator(prog.value()).generate();
    std::cout << asmOutput << std::endl;

    {
        std::fstream file("./target/out.asm", std::ios::out);
        file << asmOutput;
    }

    system("nasm -f elf64 target/out.asm -o target/out.o");
    system("ld target/out.o -o target/out");
    
    return EXIT_SUCCESS;
}