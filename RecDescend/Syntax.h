#ifndef SYNTAX_H_INCLUDED
#define SYNTAX_H_INCLUDED

enum TOKEN_TYPE
{
    CONSTANT,
    VARIABLE_TO_CREATE,
    VARIABLE_TO_USE,
    FUNCTION_TO_CREATE,
    FUNCTION_TO_USE,
    FUNC_CALL_PARAMETER,
    BIN_OPERATION,
    OPERATOR,
    ASM_INSERT,

    GLOBAL
};

enum OPERATOR_TYPE
{
    COMPLEX_OPERATOR,
    RETURN_OPERATOR,
    ASSIGNMENT_OPERATOR,
    IF_OPERATOR,
    UNTIL_OPERATOR
};

const char CONSTANT_NAME[]          = "constant";
const char FUNC_CALL_PARAM_NAME[]   = "call param";
const char BIN_OPERATION_NAME[]     = "bin operation";
const char OPERATOR_NAME[]          = "operator";
const char COMPLEX_OPERATOR_NAME[]  = "complex";

const char GLOBAL_NAME[]            = "global";

// Syntax itself, all key words are here

const std::string IF                = "if";
const std::string UNTIL             = "until";
const std::string VAR               = "var";
const std::string RETURN            = "return";
const std::string ASM               = "asm";
const std::string FUNCTION          = "function";

#endif // SYNTAX_H_INCLUDED
