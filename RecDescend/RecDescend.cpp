#include "RecDescend.h"

/// Global variable that looks at the current char in program
char* cur_pos               = nullptr;

/// Global variable that contains whole program
char* programm              = nullptr;

/// Maximum tokens amount
const int MAX_TOKENS        = 10000;

/// Global array with tokens
Token tokens[MAX_TOKENS]    = {};

/// Current token number
int cur_tok                 = 0;

/// Global error variable
int parse_error             = 0;

/// Maximum functions amount
const int MAX_FUNCTIONS     = 10000;

/// Global array with defined functions
std::string functions[MAX_FUNCTIONS]    = {};

/// Amount of functions already added (current function)
int cur_function            = 0;

/// Maximum variables in function amount
const int MAX_VARIABLES     = 10000;

/// Global array with variables
Variable variables[MAX_VARIABLES]    = {};

/// Amount of variables already added (current variable)
int cur_variable            = 0;

/// Amount of arguments in current function
int n_arguments             = 0;

// =========================================
// Additional functions supposed to be used only as inner functions

std::string CheckName()
{
    char* tmp = cur_pos;

    std::string word;
    while(  (*tmp >= 'a' && *tmp <= 'z') ||
            (*tmp >= 'A' && *tmp <= 'Z') ||
             *tmp == '_' )
    {
        word += *tmp;
        tmp++;
    }

    return word;
}

std::string GetName()
{
    std::string word;

    while(isspace(*cur_pos))    cur_pos++;

    while(  (*cur_pos >= 'a' && *cur_pos <= 'z') ||
            (*cur_pos >= 'A' && *cur_pos <= 'Z') ||
             *cur_pos == '_' )
    {
        word += *cur_pos;
        cur_pos++;
    }

    return word;
}

bool IsName(std::string to_check)
{
    if( (to_check[0] >= 'a' && to_check[0] <= 'z') ||
        (to_check[0] >= 'A' && to_check[0] <= 'Z') ||
         to_check[0] == '_' )
        return true;

    return false;
}

int FunctionNum(const std::string to_check)
{
    for(int i = 0; i < cur_function; i++)
        if(to_check == functions[i])    return i;
    return -ERROR::NOT_FOUND;
}

int VariableNum(const std::string& to_check)
{
    for(int i = 0; i < cur_variable; i++)
        if(to_check == variables[i].name_)    return i;
    return -ERROR::NOT_FOUND;
}

int ClearVariablesBuffer()
{
    for(int i = 0; i < cur_variable; i++)
        variables[i].name_ = "";
    cur_variable = 0;
    n_arguments  = 0;

    return cur_variable;
}

// =========================================

int LoadProgramm(std::string filename)
{
    programm = FileRead(filename);
    cur_pos  = programm;
    if(!programm)
    {
        std::cout << "Failed to read '" << filename << "'\n";
        return -ERROR::LOAD_ERR;
    }

    return ERROR::OK;
}

int ErrorLine()
{
    const char* tmp = programm;

    int line_num = 0;
    while(tmp != cur_pos && *tmp != '\0')
    {
        if(*tmp == '\n')    line_num++;
        tmp++;
    }

    return line_num + 1;    // As we count from 1 in IDE
}

Tree<Token>* GetFunctionCallArguments()     // Arguments grow to the right
{
    SkipSpaces(&cur_pos);

    if(*cur_pos != '(')
    {
        std::cout << "Expected '(' on line " << ErrorLine() << "\n";
        parse_error = -ERROR::INVALID_ARG;
        return nullptr;
    }
    cur_pos++;

    Tree<Token>*   first_call_parameter = nullptr;
    Tree<Token>* current_call_parameter = nullptr;
    Tree<Token>*    next_call_parameter = nullptr;

    int  times_in_loop  = 0;
    bool arguments_left = true;
    do{
        SkipSpaces(&cur_pos);
        /* */if(times_in_loop > 0 && *cur_pos != ',')
        {
            std::cout << "Expected ',' on line " << ErrorLine() << " between function arguments\n";
            parse_error = -ERROR::INVALID_ARG;
            delete first_call_parameter;
            return nullptr;
        }
        else if(times_in_loop > 0)
        {
            cur_pos++;
        }

        next_call_parameter = GetE();

        if(parse_error)
        {
            delete    next_call_parameter;
            delete   first_call_parameter;
            return nullptr;
        }

        tokens[cur_tok] = { FUNC_CALL_PARAM_NAME, FUNC_CALL_PARAMETER, 0 };
        Tree<Token>* tmp = nullptr;
        try
        {
            tmp = new Tree<Token> (tokens[cur_tok]);
            cur_tok++;
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout << "Failed to allocate memory for new function parameter\n";
            parse_error = -ERROR::UNKNOWN;
            delete next_call_parameter;
            delete first_call_parameter;
            return nullptr;
        }

        // it is the first parameter
        if(!first_call_parameter)
        {
            first_call_parameter = tmp;
            first_call_parameter->FastLeft (next_call_parameter);
            first_call_parameter->FastRigth(nullptr);
            current_call_parameter = first_call_parameter;

            // All three pointers are the same
        }
        else
        {
            current_call_parameter->FastRigth(tmp);
            current_call_parameter = current_call_parameter->Right();
            current_call_parameter->FastLeft (next_call_parameter);
            current_call_parameter->FastRigth(nullptr);

            // Do not touch first_call_parameter after its initialization
        }

        SkipSpaces(&cur_pos);
        if(*cur_pos == ')')
        {
            arguments_left = false;
            cur_pos++;
        }

        times_in_loop++;

    }while(arguments_left);

    return first_call_parameter;
}

Tree<Token>* GetFunctionCall()
{
    int func_num    = FunctionNum(GetName());
    tokens[cur_tok] = { functions[func_num], FUNCTION_TO_USE, func_num };

    Tree<Token>* function = nullptr;
    try
    {
        function = new Tree<Token> (tokens[cur_tok]);
        cur_tok++;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate memory to create Function Tree<token>\n";
        parse_error = -ERROR::UNKNOWN;
        return nullptr;
    }

    function->Right(GetFunctionCallArguments());
    function->Left (nullptr);       // gag

    return function;
}

Tree<Token>* GetVariableCall()              // <-- HERE !
{
    int var_num = VariableNum(GetName());

    tokens[cur_tok] = { variables[var_num].name_, VARIABLE_TO_USE, var_num - n_arguments };

    Tree<Token>* variable = nullptr;
    try
    {
        variable = new Tree<Token> (tokens[cur_tok]);
        cur_tok++;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate memory to create Variable Tree<token>\n";
        parse_error = -ERROR::UNKNOWN;
        return nullptr;
    }

    return variable;
}

Tree<Token>* GetFlowControl()               // Condition on the left, code on the right
{
    std::string control_type = GetName();

    // Getting condition

    SkipSpaces(&cur_pos);
    if(*cur_pos != '(')
    {
        std::cout << "Expected '(' after " << control_type << " on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    cur_pos++;

    Tree<Token>* condition = GetE();
    if(parse_error < 0)     return nullptr;

    SkipSpaces(&cur_pos);
    if(*cur_pos != ')')
    {
        std::cout << "Expected ')' after " << control_type << " on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    cur_pos++;

    // Getting code block

    Tree<Token>* code = GetOperator();
    if(parse_error < 0)
    {
        delete condition;
        return nullptr;
    }

    // Creating main node

    tokens[cur_tok] = { ((control_type == IF)? IF : UNTIL),
                        OPERATOR,
                        ((control_type == IF)? IF_OPERATOR : UNTIL_OPERATOR) };

    Tree<Token>* top_operator = nullptr;
    try
    {
        top_operator = new Tree<Token> (tokens[cur_tok]);
        cur_tok++;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate memoru for " << control_type << " operator\n";
        parse_error = -ERROR::UNKNOWN;
        delete condition;
        delete code;
        return nullptr;
    }

    top_operator->Left (condition);
    top_operator->Right(code);

    return top_operator;
}

Tree<Token>* GetVariableDeclaration()       // <-- HERE !
{
    // Skipping 'var' key word
    GetName();

    std::string new_variable_name = GetName();
    if(VariableNum(new_variable_name) >= 0)
    {
        std::cout << "Redeclaration of '" << new_variable_name << "' variable\n";
        parse_error = -ERROR::INVALID_ARG;
        return nullptr;
    }

    // Checking if semicolon presents
    SkipSpaces(&cur_pos);
    if(*cur_pos != ';')
    {
        std::cout << "Expected ';' on line " << ErrorLine() << " after variable declaration\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    cur_pos++;

    // Creating such variable
    variables[cur_variable++] = { new_variable_name, 0 };

    // Creating node

    // tokens[cur_tok] = { new_variable_name, VARIABLE_TO_CREATE, 0 };
    // tokens[cur_tok] = { new_variable_name, VARIABLE_TO_CREATE, cur_variable - n_arguments - 1 };
    tokens[cur_tok] = { new_variable_name, OPERATOR, VARIABLE_TO_CREATE };


    Tree<Token>* variable = nullptr;
    try
    {
        variable = new Tree<Token> (tokens[cur_tok]);
        cur_tok++;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate memory for variable '" << new_variable_name << "'\n";
        parse_error = -ERROR::UNKNOWN;
        return nullptr;
    }

    return variable;
}

Tree<Token>* GetAsmInsert()
{
    // Skip 'asm' key word
    GetWord(&cur_pos);

    SkipSpaces(&cur_pos);
    if(*cur_pos != '<')
    {
        std::cout << "Expected '<' after 'asm' on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    cur_pos++;

    SkipSpaces(&cur_pos);
    if(*cur_pos != '\"')
    {
        std::cout << "Expected '\"' after '<' on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    cur_pos++;

    // Reading asm insert

    std::string asm_insert = GetWordExceptSymbols(&cur_pos, "\")");

    SkipSpaces(&cur_pos);
    if(*cur_pos != '\"')
    {
        std::cout << "Expected '\"' in asm argument on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    cur_pos++;

    SkipSpaces(&cur_pos);
    if(*cur_pos != '>')
    {
        std::cout << "Expected '>' after asm on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    cur_pos++;

    // Creating node

    // tokens[cur_tok] = { asm_insert, ASM_INSERT, 0 };
    tokens[cur_tok] = { asm_insert, OPERATOR, ASM_INSERT };
    Tree<Token>* asm_node = nullptr;
    try
    {
        asm_node = new Tree<Token> (tokens[cur_tok]);
        cur_tok++;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate memory for asm insert\n";
        parse_error = -ERROR::UNKNOWN;
        return nullptr;
    }

    return asm_node;
}

Tree<Token>* GetComplexOperator()           // Nodes grow to the right
{
    // Skipping '{'
    cur_pos++;

    if(*cur_pos == '\0')    return nullptr;

    Tree<Token>*     top_operator   = nullptr;
    Tree<Token>* current_operator   = top_operator;
    Tree<Token>*    next_operator   = nullptr;
    Tree<Token>* complex_operator   = nullptr;

    int times_in_loop = 0;
    SkipSpaces(&cur_pos);
    while(*cur_pos != '}')
    {
        next_operator = GetOperator();
        if(parse_error < 0)
        {
            delete top_operator;
            return nullptr;
        }

        // Init
        tokens[cur_tok] = { COMPLEX_OPERATOR_NAME, OPERATOR, COMPLEX_OPERATOR };
        try
        {
            complex_operator = new Tree<Token> (tokens[cur_tok]);
            cur_tok++;
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout << "Failed to allocate memory for complex operator\n";
            parse_error = -ERROR::UNKNOWN;
            delete top_operator;
            delete next_operator;
            return nullptr;
        }

        // Joining nodes
        if(times_in_loop == 0)
        {
            top_operator = complex_operator;
            top_operator->FastLeft (next_operator);
            top_operator->FastRigth(nullptr);

            current_operator = top_operator;

            // Do not touch top_operator any more
        }
        else
        {
            current_operator->FastRigth(complex_operator);
            current_operator = current_operator->Right();
            current_operator->FastLeft (next_operator);
            current_operator->FastRigth(nullptr);
        }

        times_in_loop++;
        SkipSpaces(&cur_pos);
    }

    // Skipping '}'
    cur_pos++;

    return top_operator;
}

Tree<Token>* GetReturnOperator()
{
    // Skipping 'return' key word
    GetWord(&cur_pos);

    Tree<Token>* to_be_returned = GetE();
    if(parse_error < 0)     return nullptr;

    // Check if semicolon is on its place
    SkipSpaces(&cur_pos);
    if(*cur_pos != ';')
    {
        std::cout << "Expected ';' on line " << ErrorLine() << " after return statement\n";
        parse_error = -ERROR::NOT_FOUND;
        delete to_be_returned;
        return nullptr;
    }
    cur_pos++;

    // Creating return statement
    tokens[cur_tok] = { RETURN, OPERATOR, RETURN_OPERATOR };
    Tree<Token>* return_statement = nullptr;
    try
    {
        return_statement = new Tree<Token> (tokens[cur_tok]);
        cur_tok++;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate memory for return statement\n";
        delete to_be_returned;
        return nullptr;
    }

    return_statement->FastLeft (to_be_returned);
    return_statement->FastRigth(nullptr);

    return return_statement;
}

Tree<Token>* GetAssignment()
{
    if(*cur_pos == '\0')    return nullptr;

    Tree<Token>* top_operator = GetE();
    if(parse_error < 0)     return nullptr;

    Tree<Token>*    current_operator    = top_operator;
    Tree<Token>*       next_operator    = nullptr;
    Tree<Token>* assignment_operator    = nullptr;

    // Probably assignment is empty - nothing to be assigned, then just return expr we got
    int times_in_loop = 0;
    SkipSpaces(&cur_pos);
    while(*cur_pos == '=')
    {
        cur_pos++;

        Tree<Token>* next_operator = GetE();
        if(parse_error < 0)
        {
            std::cout << "Failed to get assignment.\n";
            delete top_operator;
            return nullptr;
        }
        if(next_operator == nullptr)
        {
            std::cout << "Expected value after '=' on line " << ErrorLine() << "\n";
            parse_error = -ERROR::INVALID_ARG;
            delete top_operator;
            return nullptr;
        }

        // Creating assignment node
        tokens[cur_tok] = { OPERATOR_NAME, OPERATOR, ASSIGNMENT_OPERATOR };
        try
        {
            assignment_operator = new Tree<Token> (tokens[cur_tok]);
            cur_tok++;
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout << "Failed to allocate memory for assignment operator\n";
            parse_error = -ERROR::UNKNOWN;
            delete top_operator;
            delete next_operator;
            return nullptr;
        }

        // Checking if this is first assignment
        if(times_in_loop == 0)
        {
            Tree<Token>* tmp = top_operator;

            top_operator = assignment_operator;
            top_operator->FastLeft (tmp);
            top_operator->FastRigth(next_operator);
            current_operator = top_operator;
        }
        else
        {
            Tree<Token>* tmp = current_operator->Right();

            current_operator->FastRigth(assignment_operator);
            current_operator = current_operator->Right();
            current_operator->FastLeft (tmp);
            current_operator->FastRigth(next_operator);

            // Do not touch top_operator more
        }

        SkipSpaces(&cur_pos);
        times_in_loop++;
    }

    // Checking if semicolon presents
    SkipSpaces(&cur_pos);
    if(*cur_pos != ';')
    {
        std::cout << "Expected ';' on line " << ErrorLine() << " after expression\n";
        parse_error = -ERROR::NOT_FOUND;
        delete top_operator;
        return nullptr;
    }
    cur_pos++;

    return top_operator;
}

int GetFunctionDeclareArguments()
{
    SkipSpaces(&cur_pos);
    if(*cur_pos != '(')
    {
        std::cout << "Expected '(' after function declaration on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return -ERROR::NOT_FOUND;
    }
    cur_pos++;

    int  times_in_loop  = 0;
    bool arguments_left = true;
    std::string parameter_name;
    do{
        SkipSpaces(&cur_pos);
        /* */if(times_in_loop > 0 && *cur_pos != ',')
        {
            std::cout << "Expected ',' on line " << ErrorLine() << " between function arguments\n";
            parse_error = -ERROR::INVALID_ARG;
            return -ERROR::INVALID_ARG;
        }
        else if(times_in_loop > 0)
        {
            cur_pos++;
        }

        // Taking new name
        parameter_name = GetName();
        if(parameter_name.length() == 0)
        {
            SkipSpaces(&cur_pos);
            if(*cur_pos == ')')
            {
                // Empty brackets
                cur_pos++;
                return times_in_loop;   // Equivalent to 'return 0'
            }
            else
            {
                std::cout << "Expected argument in function declaration on line " << ErrorLine() << "\n";
                parameter_name = -ERROR::NOT_FOUND;
                return -ERROR::NOT_FOUND;
            }
        }
        if(VariableNum(parameter_name) >= 0)
        {
            std::cout << "Redeclaration of '" << parameter_name << "' on line " << ErrorLine() << "\n";
            parameter_name = -ERROR::NOT_FOUND;
            return -ERROR::NOT_FOUND;
        }

        variables[cur_variable++] = { parameter_name, 0 };

        SkipSpaces(&cur_pos);
        if(*cur_pos == ')')
        {
            arguments_left = false;
            cur_pos++;
        }

        times_in_loop++;
        n_arguments++;

    }while(arguments_left);

    return times_in_loop;
}

Tree<Token>* BuildSyntaxTree()
{
    assert(cur_pos);

    Tree<Token>* syntax_tree = GetG0();

    delete [] programm;
    return syntax_tree;
}

Tree<Token>* GetN()
{
    if(parse_error < 0)     return nullptr;

    char* beginning = cur_pos;
    SkipSpaces(&cur_pos);
    std::string word = GetWordExceptSymbols(&cur_pos, " \t\n\v\r\f(){};,");
    int value = 0;

    try
    {
        value = stoi(word);
    }
    catch(...)
    {
        std::cout << "Failed to transform to integer: '" << word << "' on line " << ErrorLine() << "\n";
        cur_pos = beginning;
        return nullptr;
    }

    // Init constant
    Tree<Token>* tree   = nullptr;
    tokens[cur_tok]   = {CONSTANT_NAME, CONSTANT, value};
    try
    {
        tree  = new Tree<Token> (tokens[cur_tok++]);
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate Tree<Token> and Token in GetN()\n";
        parse_error = -ERROR::UNKNOWN;
        return nullptr;
    }

    return tree;
}

Tree<Token>* GetP()                         // ( expr ), ( ), 123, VarName, FuncName
{
    if(parse_error < 0)     return nullptr;

    Tree<Token>* elem = nullptr;
    SkipSpaces(&cur_pos);

    if(*cur_pos == '\0')
        return nullptr;
    if(*cur_pos == '(')
    {
        cur_pos++;

        elem = GetE();

        if(*cur_pos != ')')
        {
            std::cout << "Error in line " << ErrorLine() << ": expected ')'\n";
            parse_error =   -ERROR::INVALID_ARG;
            delete elem;
            return nullptr;
        }
        cur_pos++;
    }
    else if(IsName(CheckName()))
    {
        if(FunctionNum(CheckName()) >= 0)     // Try word. Maybe Function -> GetFunctionCall()
        {
            elem = GetFunctionCall();
        }
        else if(VariableNum(CheckName()) >= 0)     // Try word. Maybe Variable -> GetVariable()
        {
            elem = GetVariableCall();
        }
        else
        {
            std::cout << "Unknown variable or function: '" << CheckName() << "' on line " << ErrorLine() << "\n";
            parse_error = -ERROR::UNKNOWN;
            return nullptr;
        }
    }
    else
    {
        elem = GetN();
    }

    // In case of fault just returning. Everything is already printed
    if(parse_error < 0)
    {
        delete elem;
        return nullptr;
    }

    return elem;
}

Tree<Token>* GetT()                         // (expr) * (expr)
{
    if(*cur_pos == '\0')    return nullptr;

    Tree<Token>* first_expression = GetP();
    if(parse_error < 0)
    {
        delete first_expression;
        return nullptr;
    }

    Tree<Token>* current_expression = first_expression;
    Tree<Token>*    next_expression = nullptr;
    Tree<Token>*     tmp_expression = nullptr;

    int times_in_loop = 0;
    SkipSpaces(&cur_pos);
    while(*cur_pos == '*' || *cur_pos == '/')
    {
        int operation = *cur_pos;
        cur_pos++;

        // Getting expression
        next_expression = GetP();
        if(parse_error < 0)
        {
            delete first_expression;
            return nullptr;
        }

        // Creating new node
        tokens[cur_tok] = { BIN_OPERATION_NAME, BIN_OPERATION, operation };
        try
        {
            tmp_expression = new Tree<Token> (tokens[cur_tok]);
            cur_tok++;
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout << "Failed to allocate memory in GetT()\n";
            parse_error = -ERROR::UNKNOWN;
            delete first_expression;
            delete  next_expression;
            return nullptr;
        }

        // Placing nodes to the right places
        if(times_in_loop == 0)
        {
            // Nothing is set correct way
            Tree<Token>* tmp = first_expression;

            first_expression = tmp_expression;
            first_expression->Left (tmp);
            first_expression->Right(next_expression);
            current_expression = first_expression;
        }
        else
        {
            // Habitual algorythm
            Tree<Token>* tmp = current_expression->Right();

            current_expression->FastRigth(tmp_expression);
            current_expression = current_expression->Right();
            current_expression->Left (tmp);
            current_expression->Right(next_expression);
        }

        SkipSpaces(&cur_pos);
        times_in_loop++;
    }

    return first_expression;
}

Tree<Token>* GetE()                         // GetT + GetT
{
    if(*cur_pos == '\0')    return nullptr;

    Tree<Token>* first_expression = GetT();
    if(parse_error < 0)
    {
        delete first_expression;
        return nullptr;
    }

    Tree<Token>* current_expression = first_expression;
    Tree<Token>*    next_expression = nullptr;
    Tree<Token>*     tmp_expression = nullptr;

    int times_in_loop = 0;
    SkipSpaces(&cur_pos);
    while(  *cur_pos == '+' || *cur_pos == '-' ||
            *cur_pos == '>' || *cur_pos == '<' || *cur_pos == '~')  // '~' is equivalent to '=='
    {
        int operation = *cur_pos;
        cur_pos++;

        // Getting expression
        next_expression = GetT();
        if(parse_error < 0)
        {
            delete first_expression;
            return nullptr;
        }

        // Creating new node
        tokens[cur_tok] = { BIN_OPERATION_NAME, BIN_OPERATION, operation };
        try
        {
            tmp_expression = new Tree<Token> (tokens[cur_tok]);
            cur_tok++;
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout << "Failed to allocate memory in GetE()\n";
            parse_error = -ERROR::UNKNOWN;
            delete first_expression;
            delete  next_expression;
            return nullptr;
        }

        // Placing nodes to the right places
        if(times_in_loop == 0)
        {
            // Nothing is set correct way
            Tree<Token>* tmp = first_expression;

            first_expression = tmp_expression;
            first_expression->Left (tmp);
            first_expression->Right(next_expression);
            current_expression = first_expression;
        }
        else
        {
            // Habitual algorythm
            Tree<Token>* tmp = current_expression->Right();

            current_expression->FastRigth(tmp_expression);
            current_expression = current_expression->Right();
            current_expression->Left (tmp);
            current_expression->Right(next_expression);
        }

        SkipSpaces(&cur_pos);
        times_in_loop++;
    }

    return first_expression;
}

Tree<Token>* GetOperator()                  // Assignment must be checked when creating asm file !!!
{
    if(*cur_pos == '\0')    return nullptr;

    // So here we go
    Tree<Token>* top_operator = nullptr;

    SkipSpaces(&cur_pos);
    std::string command = CheckName();

    /* */if(command == IF || command == UNTIL)
    {
        top_operator = GetFlowControl();
    }
    else if(command == VAR)
    {
        top_operator = GetVariableDeclaration();
    }
    else if(command == RETURN)
    {
        top_operator = GetReturnOperator();
    }
    else if(command == ASM)
    {
        top_operator = GetAsmInsert();
    }
    else if(*cur_pos == '{')
    {
        top_operator = GetComplexOperator();
    }
    else
    {
        top_operator = GetAssignment();
    }

    return top_operator;
}

Tree<Token>* GetFunction()
{
    // Shit... So close

    // Getting function's name

    std::string function_name = GetName();
    if(function_name.length() == 0)
    {
        std::cout << "Expected function's name after 'function' on line " << ErrorLine() << "\n";
        parse_error = -ERROR::NOT_FOUND;
        return nullptr;
    }
    if(FunctionNum(function_name) >= 0)
    {
        std::cout << "Redeclaration of '" << function_name << " on line " << ErrorLine() << "\n";
        parse_error = -ERROR::INVALID_ARG;
        return nullptr;
    }

    functions[cur_function++] = function_name;

    // Get arguments list

    int n_arguments = GetFunctionDeclareArguments();

    // Get body

    Tree<Token>* function_body = GetOperator();
    if(parse_error)     return nullptr;

    // Join them

    tokens[cur_tok] = { function_name, FUNCTION_TO_CREATE, n_arguments };
    Tree<Token>* function = nullptr;
    try
    {
        function = new Tree<Token> (tokens[cur_tok]);
        cur_tok++;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate memory for function '" << function_name << "'\n";
        parse_error = -ERROR::UNKNOWN;
        delete function_body;
        return nullptr;
    }

    function->Left (function_body);
    function->Right(nullptr);

    return function;
}

Tree<Token>* GetG0()
{
    Tree<Token>*         top_global = nullptr;
    Tree<Token>*      next_function = nullptr;
    Tree<Token>*     current_global = nullptr;
    Tree<Token>* tmp_global_handler = nullptr;

    int times_in_loop = 0;
    while(GetWord(&cur_pos) == FUNCTION)
    {
        next_function = GetFunction();
        if(parse_error < 0)     return nullptr;
        ClearVariablesBuffer();

        // Creating new node

        tokens[cur_tok] = { GLOBAL_NAME, GLOBAL, 0 };
        try
        {
            tmp_global_handler = new Tree<Token> (tokens[cur_tok]);
            cur_tok++;
        }
        catch(const std::bad_alloc& ex)
        {
            std::cout << "Failed to allocate memory for global node\n";
            delete next_function;
            return nullptr;
        }

        //  gathering all together

        if(times_in_loop == 0)
        {
            top_global = tmp_global_handler;
            top_global->Left (next_function);
            top_global->Right(nullptr);

            current_global = top_global;
        }
        else
        {
            current_global->Right(tmp_global_handler);
            current_global = current_global->Right();
            current_global->Left (next_function);
            current_global->Right(nullptr);
        }

        if(parse_error < 0)
        {
            delete top_global;
            return nullptr;
        }

        times_in_loop++;
    }

    SkipSpaces(&cur_pos);
    if(*cur_pos != '\0')
    {
        std::cout << "Weird syntax on line " << ErrorLine() << "\n";
        parse_error = -ERROR::UNKNOWN;
        delete top_global;
        return nullptr;
    }

    return top_global;
}

int Dump(const Token& a, FILE* output)
{
    return fprintf(output, "{ %s | %d | %d }", a.Name().c_str(), a.Type(), a.Value());
}

