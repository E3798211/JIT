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
std::string variables[MAX_VARIABLES]    = {};

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

int FunctionNum(const std::string& to_check)
{
    for(int i = 0; i <= cur_function; i++)
        if(to_check == functions[i])    return i;
    return -ERROR::NOT_FOUND;
}

int VariableNum(const std::string& to_check)
{
    for(int i = 0; i <= cur_variable; i++)
        if(to_check == variables[i])    return i;
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

    do{
        next_call_parameter = GetE();

        if(parse_error)
        {
            delete    next_call_parameter;
            delete current_call_parameter;
            if(first_call_parameter != current_call_parameter)  // Preventing double free
                delete   first_call_parameter;
            return nullptr;
        }

        // it is the first parameter
        if(!first_call_parameter)
        {
            current_call_parameter = next_call_parameter;
              first_call_parameter = current_call_parameter;

            // All three pointers are the same
        }
        else
        {
            current_call_parameter->Right(next_call_parameter);
            current_call_parameter = current_call_parameter->Right();

            // Do not touch first_call_parameter after its initialization
        }

    }while(next_call_parameter != nullptr);

    if(*cur_pos != ')')
    {
        std::cout << "Expected ')' on line " << ErrorLine() << "\n";
        parse_error = -ERROR::INVALID_ARG;

        delete first_call_parameter;

        return nullptr;
    }
    cur_pos++;

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

int BuildSyntaxTree()           // <-- TO BE FINISHED
{
    assert(cur_pos);

    while(*cur_pos != '\0')
    {
        // GetFunction
        Tree<Token>* cur = GetN();

        // If error occured
        if(parse_error < 0)
        {
            std::cout << "Error occured on line " << ErrorLine() << "\n";
            delete cur;
            delete [] programm;
            return -ERROR::INVALID_ARG;
        }

        // If buffer overflown
        if(cur_tok >= MAX_TOKENS)
        {
            std::cout << "Too many tokens created. Please create smaller programm :)\n";
            delete cur;
            delete [] programm;
            return -ERROR::OVERFLOW;
        }

        cur_pos++;

        // MAYBE CONCATENATE THEM???
    }

    delete [] programm;
    return ERROR::OK;

    // RETURN TREE
}

Tree<Token>* GetN()
{
    if(parse_error < 0)     return nullptr;

    std::string word = GetWord(&cur_pos);
    int value = 0;

    try
    {
        value = stoi(word);
    }
    catch(...)
    {
        std::cout << "Failed to transform to integer: '" << word << "' on line " << ErrorLine() << "\n";
        parse_error =   -ERROR::INVALID_ARG;
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

    if(*cur_pos == '(')
    {
        cur_pos++;

        // Check if theese are empty brackets or not
        SkipSpaces(&cur_pos);
        if(*cur_pos == ')')
        {
            std::cout << "Empty () in GetP on line " << ErrorLine() << ". Returning nullptr without error\n";
            cur_pos++;
            return nullptr;
        }
        else
        {
            elem = GetE();
        }

        // In case of fault just returning. Everything is already printed
        if(parse_error < 0)
        {
            delete elem;
            return nullptr;
        }

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
        // int var_num  = VariableNum(GetName());
        //
    }
    else
    {
        elem = GetN();
    }

    return elem;
}

Tree<Token>* GetT()     // (expr) * (expr)
{
    //
}

Tree<Token>* GetE()     // GetT + GetT
{
    // Empty expr is ok
}

