
#include "CFG.h"
#include "AST.h"

struct Parser {
    CFG grammar;
    AST ast;

    // Given a file path containing grammar specifications for the language,
    // generate the CFG object which represents it
    void load_grammar(string);
    
    // Given a stream of tokens, generate an AST which represents it
    // Store it within the ast variable
    void generate_AST(const vector<Token>&);

};