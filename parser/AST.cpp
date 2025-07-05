
#include <stdexcept>
#include <iostream>
#include <variant>

#include "AST.h"

using std::stack;
using std::variant;
using std::get_if;
using std::get;
using std::runtime_error;

void construct_AST_helper(ASTNode* root) {
    for (ASTNode* child : root->children) {
        construct_AST_helper(child);
    }

    if (root->node_type == AST_NODE_TYPE::FUNCTION_NODE) {
        ASTFunctionNode* func_node = dynamic_cast<ASTFunctionNode*>(root);

        for (ASTNode* child : root->children) {
            if (child->node_type == AST_NODE_TYPE::IDENT_NODE) {
                func_node->function_name = dynamic_cast<ASTIdentNode*>(child)->identifier;
            }
        }
    } else if (root->node_type == AST_NODE_TYPE::TYPE_NODE) {
        ASTTypeNode* type_node = dynamic_cast<ASTTypeNode*>(root);
        ASTTempNode* temp = dynamic_cast<ASTTempNode*>(type_node->children.at(0));

        type_node->type_name = temp->data;
    }

    auto it = root->children.begin();

    while (it != root->children.end()) {
        ASTNode* cur = *it;

        if (cur->node_type == AST_NODE_TYPE::TEMP_NODE || cur->node_type == AST_NODE_TYPE::TYPE_NODE || cur->node_type == AST_NODE_TYPE::IDENT_NODE) {
            it = root->children.erase(it, it + 1);
            delete cur;
        } else {
            it++;
        }

    }
}

void AST::construct_AST_from_parse_tree() {

    construct_AST_helper(root);
}

void AST::construct_leaf_node(Token& token, Symbol* symbol, stack<ASTNode*>& ast_stack) {
    switch (symbol->corresponding_token) {
        case TOKEN_TYPE::INT_CONST:
            ast_stack.push(new ASTIntConstNode(std::stoi(token.value)));
            break;
        case TOKEN_TYPE::IDENTIFIER:
            ast_stack.push(new ASTIdentNode(token.value));
            break;
        default:
            ast_stack.push(new ASTTempNode(symbol->symbol, symbol->corresponding_token));
            break;
    }
}

void AST::construct_production_node(int production_index, stack<ASTNode*>& ast_stack) {
    Rule rule = grammar.productions.at(production_index);
    string production_symbol = rule.symbol->symbol;

    ASTNode* parent_node;

    if (production_symbol == "<root>") {
        parent_node = new ASTRootNode();
    } else if (production_symbol == "<function>") {
        parent_node = new ASTFunctionNode();
    } else if (production_symbol == "<return>") {
        parent_node = new ASTReturnNode();
    } else if (production_symbol == "<type>") {
        parent_node = new ASTTypeNode();
    }

    for (int i = 0; i < rule.production_rule.size(); i++) {
        parent_node->children.push_back(ast_stack.top());
        ast_stack.pop();
    }

    ast_stack.push(parent_node);
}

void AST::construct_parse_tree(const vector<Token>& token_stream) {
    stack<ASTNode*> ast_stack;
    stack<std::variant<Symbol*, int>> symbol_stack;

    int i = 0;

    symbol_stack.push(grammar.symbols.at("$"));
    symbol_stack.push(grammar.symbols.at("<root>"));

    while (true) {

        if (get_if<Symbol*>(&symbol_stack.top())) {
            Symbol* stack_top = *get_if<Symbol*>(&symbol_stack.top());

            if (stack_top->is_terminal) {
                // Check to see if we have reached end of input and we have reached the end of input token as well
                if (stack_top == grammar.symbols.at("$")) {
                    // If for some reason we reach the end of input symbol on the stack but there are still tokens left to use, then there is a syntax error
                    if (i == token_stream.size() - 1) {
                        break;
                    } else {
                        throw runtime_error("Syntax error");
                    }
                }

                // If terminal symbol, consume token from the input string and pop
                Token cur_token = token_stream.at(i);
                
                // Check if out token's TOKEN_TYPE matches with the stack top's symbol's TOKEN_TYPE
                if (TERMINAL_MAP.at(stack_top->symbol) == cur_token.token_type) {
                    // Consume input symbol and pop from stack
                    i++;

                    construct_leaf_node(cur_token, stack_top, ast_stack);
                    symbol_stack.pop();
                } else {
                    throw runtime_error("Syntax error");
                    return;
                }
            } else {
                // If terminal symbol, consume token from the input string and pop
                Token cur_token = token_stream.at(i);
                
                Symbol* cur_symbol = stack_top;
                Symbol* next_token_symbol = grammar.token_to_symbol(cur_token);

                if (grammar.parse_table.find(cur_symbol) == grammar.parse_table.end()) {
                    throw runtime_error("Syntax error");
                    return;
                }

                if (grammar.parse_table.at(cur_symbol).find(next_token_symbol) == grammar.parse_table.at(cur_symbol).end()) {
                    throw runtime_error("Syntax error");
                    return;
                }

                int production_number = grammar.parse_table.at(cur_symbol).at(next_token_symbol);
                vector<Symbol*> production = grammar.productions.at(production_number).production_rule;

                symbol_stack.pop();

                // Add to AST stack
    
                symbol_stack.push(production_number);

                for (int j = production.size() - 1; j >= 0; j--) {
                    symbol_stack.push(production.at(j));
                }

            }
        } else {
            // Pop from AST stack and push a new AST node based on the rule
            construct_production_node(get<int>(symbol_stack.top()), ast_stack);
            symbol_stack.pop();
        }


    }

    root = ast_stack.top();

}