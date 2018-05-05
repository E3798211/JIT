#ifndef SYNTAX_H_INCLUDED
#define SYNTAX_H_INCLUDED

enum TOKEN_TYPE
{
    CONSTANT,
    VARIABLE_TO_CREATE,
    VARIABLE_TO_USE,
    FUNCTION,
    FUNC_CALL_PARAMETER,
    BIN_OPERATION,
    OPERATOR,
    COMPLEX_OPERATOR,
};

const char CONSTANT_NAME[]          = "constant";
const char FUNC_CALL_PARAM_NAME[]   = "call param";
const char BIN_OPERATION_NAME[]     = "bin operation";
const char OPERATOR_NAME[]          = "operator";
const char COMPLEX_OPERATOR_NAME[]  = "complex";


// Syntax itself, all key words are here

const std::string IF                = "if";
const std::string UNTIL             = "until";
const std::string VAR               = "var";
const std::string RETURN            = "return";
const std::string ASM               = "asm";

#endif // SYNTAX_H_INCLUDED
