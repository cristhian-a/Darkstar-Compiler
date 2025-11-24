#include <iostream>
#include <fstream>
#include <optional>
#include <vector>

#include "./arena.hpp"
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

    ArenaAllocator arena(4 * 1024 * 1024);
    std::optional<node::Prog*> prog = Parser(std::move(tokens), arena).parse_prog();

    if (!prog.has_value()) {
        std::cerr << "No valid program statements!\n";
        return EXIT_FAILURE;
    }

    {
        Generator generator(prog.value());
        std::fstream file("./target/out.asm", std::ios::out);
        file << generator.generate();
    }

    arena.reset();

    system("nasm -f elf64 target/out.asm -o target/out.o");
    system("ld target/out.o -o target/out");
    
    return EXIT_SUCCESS;
}