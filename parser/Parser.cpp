
#include "Parser.h"

void Parser::load_grammar(string file_path) {
    grammar.load_data(file_path);

    grammar.construct_FIRST_sets();
    grammar.construct_FOLLOW_sets();
    grammar.construct_parse_table();

    ast = AST(grammar);
}

void Parser::generate_AST(const vector<Token>& token_stream) {
    ast.construct_parse_tree(token_stream);
    ast.construct_AST_from_parse_tree();
}