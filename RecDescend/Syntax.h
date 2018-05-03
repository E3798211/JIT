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

#endif // SYNTAX_H_INCLUDED
