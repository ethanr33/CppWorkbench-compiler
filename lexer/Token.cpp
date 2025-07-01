
#include "Token.h"

ostream& operator<<(ostream& out, TOKEN_TYPE token) {
    out << static_cast<int>(token);
    return out;
}

TOKEN_TYPE determine_token_type(const string& token) {
    if (token == "int") {
        return TOKEN_TYPE::KW_INT;
    } else if (token == "return") {
        return TOKEN_TYPE::KW_RETURN;
    } else {
        return TOKEN_TYPE::IDENTIFIER;
    }
}