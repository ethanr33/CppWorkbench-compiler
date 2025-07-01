
#pragma once

#include <vector>

#include "Token.h"

using std::vector;

struct Lexer {
    string program_data;
    std::vector<Token> token_stream;    

    Lexer() : program_data("") {};

    void load_program(const string&);
    void lex();
};