
#include <iostream>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "codegen/AssemblyGenerator.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: ./compiler <file name>" << endl;
        return 1;
    }

    Lexer lexer = Lexer();
    lexer.load_program(argv[1]);
    lexer.lex();

    Parser parser;
    parser.load_grammar("parser/grammar.txt");
    parser.generate_AST(lexer.token_stream);

    AssemblyGenerator generator;
    generator.convert_AST_to_assembly(parser.ast);
    generator.output_assembly_to_file("out.asm");

    return 0;
}