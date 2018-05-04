#ifndef SYNTAX_H_INCLUDED
#define SYNTAX_H_INCLUDED

enum TOKEN_TYPE
{
    CONSTANT,
    VARIABLE,
    FUNCTION,
    FUNC_CALL_PARAMETER,
    BIN_OPERATION,
    OPERATOR
};

const char CONSTANT_NAME[]          = "constant";
const char FUNC_CALL_PARAM_NAME[]   = "call param";
const char BIN_OPERATION_NAME[]     = "bin operation";


// Syntax itself, all key words are here

const std::string IF                = "if";
const std::string UNTIL             = "until";
const std::string VAR               = "var";
const std::string RETURN            = "return";
const std::string ASM               = "asm";

#endif // SYNTAX_H_INCLUDED
