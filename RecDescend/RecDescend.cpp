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

/// Amount of functions already added (current function)
int cur_variable            = 0;

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
    while(  (*cur_pos >= 'a' && *cur_pos <= 'z') ||
            (*cur_pos >= 'A' && *cur_pos <= 'Z') ||
             *cur_pos == '_' )
    {
        word += *cur_pos;
        cur_pos++;
    }

    return word;
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

    return line_num;
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
    tokens[cur_tok] = { functions[func_num], FUNCTION, func_num };

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

Tree<Token>* GetVariableCall()
{
    int var_num = VariableNum(GetName());

    tokens[cur_tok] = { variables[var_num].name_, VARIABLE, variables[var_num].value_ };

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

Tree<Token>* BuildSyntaxTree()           // <-- TO BE FINISHED
{
    assert(cur_pos);

    functions[cur_function++] =   "FFunction";
    variables[cur_variable++] = { "VVariable", 37 };

    tokens[cur_tok] = {"GLOBAL", OPERATOR, 0};
    Tree<Token>* first = new Tree<Token> (tokens[cur_tok++]);
    Tree<Token>* current = first;

    while(*cur_pos != '\0')
    {
        // GetFunction
        Tree<Token>* cur = GetE();

        current->FastLeft(cur);

        tokens[cur_tok] = { "GLOBAL", OPERATOR, 0 };
        current->Right(new Tree<Token> (tokens[cur_tok++]));
        current = current->Right();

        // If error occured
        if(parse_error < 0)
        {
            std::cout << "Error occured on line " << ErrorLine() << "\n";
            delete first;
            delete [] programm;
            return nullptr;
        }

        // If buffer overflown
        if(cur_tok >= MAX_TOKENS)
        {
            std::cout << "Too many tokens created. Please create smaller programm :)\n";
            // delete cur;
            delete first;
            delete [] programm;
            return nullptr;
        }

        cur_pos++;

        // MAYBE CONCATENATE THEM???
    }

    delete [] programm;
    return first;
}

Tree<Token>* GetN()
{
    if(parse_error < 0)     return nullptr;

    char* beginning = cur_pos;
    std::string word = GetWordExceptSymbols(&cur_pos, " \t\n(){};,");
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

Tree<Token>* GetP()     // ( expr ), ( ), 123, VarName, FuncName
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
    else if(FunctionNum(CheckName()) >= 0)     // Try word. Maybe Function -> GetFunctionCall()
    {
        elem = GetFunctionCall();
    }
    else if(VariableNum(CheckName()) >= 0)     // Try word. Maybe Variable -> GetVariable()
    {
        elem = GetVariableCall();
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

int Dump(const Token& a, FILE* output)
{
    return fprintf(output, "{ %s | %d | %d }", a.Name().c_str(), a.Type(), a.Value());
}

Tree<Token>* GetT()     // (expr) * (expr)
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

Tree<Token>* GetE()     // GetT + GetT
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

Tree<Token>* GetOperator()
{
    Tree<Token>* elem = GetE();
    std::cout << "\t\tIn GetOp() before sc check: '" << *cur_pos << "'\n";
    if(*cur_pos != ';')
    {
        std::cout << "Expected cemicolon on line " << ErrorLine() << "\n";
        return nullptr;
    }
    return elem;
}