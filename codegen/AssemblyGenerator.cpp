
#include <fstream>
#include <stdexcept>

#include "AssemblyGenerator.h"

using std::ofstream;

/*
    Currently, the compiler is only capable of compiling a program with a simple int main() function with no parameters that returns an integer
    When more functionality is 
*/

void AssemblyGenerator::convert_AST_to_assembly(AST& ast) {

    ASTNode* root = ast.root;

    // Locate our entry point, int main()

    ASTFunctionNode* entry_point = nullptr;

    for (ASTNode* child : root->children) {
        if (child->node_type == AST_NODE_TYPE::FUNCTION_NODE) {
            ASTFunctionNode* potential_entry_point = dynamic_cast<ASTFunctionNode*>(child);

            if (potential_entry_point->function_name == "main") {
                entry_point = potential_entry_point;
                break;
            }
        }
    }

    // If there is no entry point, then the program shouldn't compile (for now)
    if (entry_point == nullptr) {
        throw std::runtime_error("No entry point main() found");
    }

    // Get main()'s return value

    int return_val;
    bool return_node_found = false;

    for (ASTNode* child : entry_point->children) {
        if (child->node_type == AST_NODE_TYPE::RETURN_NODE) {
            return_node_found = true;

            // Return nodes will only have one child, the return value
            ASTNode* return_val_node = child->children.at(0);

            if (return_val_node->node_type == AST_NODE_TYPE::INT_CONST_NODE) {
                return_val = dynamic_cast<ASTIntConstNode*>(return_val_node)->value;
                break;
            } else {
                throw std::runtime_error("main() should return an int");
            }
        }
    }

    if (!return_node_found) {
        throw std::runtime_error("No return statement found in main()");
    }

    // If main() is found and it has a valid return value, then this is a valid program, so generate the assembly

    // Define entry point
    generated_assembly = "[BITS 64]\nglobal _start\n_start:\n";
    
    // Add code for return value
    // Since we are putting code directly in _start, we need to use the exit syscall
    // if we use ret it will pop off an empty stack which would cause undefined behavior
    generated_assembly += "mov rax, 60\nmov rdi, " + std::to_string(return_val) + "\nsyscall";
}

void AssemblyGenerator::output_assembly_to_file(const string& file_name) const {
    ofstream assembly_file(file_name);

    if (!assembly_file.is_open()) {
        throw std::runtime_error("Failed to open output file.");
    }

    assembly_file << generated_assembly;

    assembly_file.close();
}