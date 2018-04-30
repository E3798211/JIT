#include "RecDescend.h"

/// Global variable that looks at the current char in program
char* cur_pos         = nullptr;

/// Global variable that contains whole program
char* programm        = nullptr;

/// Maximum tokens amount
const int MAX_TOKENS        = 10000;

/// Global array with tokens
Token tokens[MAX_TOKENS]    = {};

/// Current token number
int cur_tok                 = 0;

/// Global error variable
int parse_error             = 0;

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

int BuildSyntaxTree()
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
            delete [] programm;
            return -ERROR::INVALID_ARG;
        }

        // If buffer overflown
        if(cur_tok >= MAX_TOKENS)
        {
            std::cout << "Too many tokens created. Please create smaller programm :)\n";
            delete [] programm;
            return -ERROR::OVERFLOW;
        }

        cur_pos++;
    }

    delete [] programm;
    return ERROR::OK;
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
        return          nullptr;
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

Tree<Token>* GetP()
{
    if(parse_error < 0)     return nullptr;

    Tree<Token>* elem = nullptr;
    SkipSpaces(&cur_pos);
    if(*cur_pos == '(')
    {
        cur_pos++;
        elem = GetE();

        // In case of fault just returning. Everything is already printed
        if(parse_error < 0)     return nullptr;

        if(*cur_pos != ')')
        {
            std::cout << "Error in line " << ErrorLine() << ": expected ')'\n";
            parse_error =   -ERROR::INVALID_ARG;
            return nullptr;
        }
        cur_pos++;
    }
    else if (false)     // Try word. Maybe Function -> GetFunctionCall(), maybe Variable - GetVariable()
    {
        //
    }
    else
    {
        elem = GetN();
    }

    return elem;
}

Tree<Token>* GetT()
{
    //
}

Tree<Token>* GetE()
{
    //
}

