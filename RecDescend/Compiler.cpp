#include "Compiler.h"

/// Variable to create different names
/**
    Incremented immideately after usage, this way every function can create unique labels
*/
int different_label_names = 0;



int Compile(const std::string source_code_filename)
{
    // Loading programm
    if(LoadProgramm(source_code_filename))      return -ERROR::FILE_NOT_OPENED;

    // Building tree
    Tree<Token>* syntax_tree = BuildSyntaxTree();
    if(!syntax_tree)                            return -ERROR::UNKNOWN;

    // Printing picture
    syntax_tree->CreateDotOutput(Dump);

    // Getting round the tree
    TreeToAsm(syntax_tree);

    delete syntax_tree;

    return ERROR::OK;
}

int TreeToAsm(Tree<Token>* syntax_tree)
{
    assert(syntax_tree);

    FILE* output = fopen(COMPILED_CODE_FILENAME, "w");
    if(!output)
    {
        std::cout << "Failed to open '" << COMPILED_CODE_FILENAME << "'\n";
        return -ERROR::FILE_NOT_OPENED;
    }

    int status = PrintGlobal(syntax_tree, output);
    fclose(output);

    return status;
}

int PrintComplex(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    while(root)
    {
        PrintOperator(root->Left(), output);
        root = root->Right();
    }

    return ERROR::OK;
}

int PrintIf(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    int label_num = different_label_names++;
    fprintf(output, "#IF START\n");

    // Printing Condition
    PrintExpression(root->Left(), output);

    fprintf(output, "mov  bx , 0  \n");
    fprintf(output, "cmp  ax , bx \n");
    fprintf(output, "je   $endif_%d\n",         label_num);

    // Printing Code
    PrintOperator(root->Right(), output);

    fprintf(output, "%%endif_%d   \n",          label_num);
    fprintf(output, "#IF ENDED    \n");

    return ERROR::OK;
}

int PrintUntil(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    int label_num = different_label_names++;
    fprintf(output, "#UNTIL START\n");

    // Skipping over code
    fprintf(output, "jmp $conditionuntil_%d\n", label_num);

    // Starting loop
    fprintf(output, "%%startuntil_%d\n",        label_num);

    // Printing Code
    PrintOperator(root->Right(), output);

    // Printing Condition
    fprintf(output, "%%conditionuntil_%d\n",    label_num);

    PrintExpression(root->Left(), output);

    // Checking Condition
    fprintf(output, "mov  bx , 0 \n");
    fprintf(output, "cmp  ax , bx\n");
    fprintf(output, "jne  $startuntil_%d\n",    label_num);

    fprintf(output, "#UNTIL ENDED   \n");

    return ERROR::OK;
}

int PrintVariableToCreate(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    fprintf(output, "mov  cx , 1\n");
    fprintf(output, "sub  sp , cx\n");

    return ERROR::OK;
}

int PrintReturn(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    // Just printing return statement
    PrintExpression(root->Left(), output);
    // Duplicating return statement
    fprintf(output, "mov  sp , bp\n");
    fprintf(output, "pop  bp\n");
    fprintf(output, "ret\n");

    return ERROR::OK;
}

int PrintAsmInsert(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    fprintf(output, "# USR ASM INSERT BEGIN\n");
    fprintf(output, "%s\n", root->Data().Name().c_str());
    fprintf(output, "# USR ASM INSERT ENDED\n");

    return ERROR::OK;
}

int PrintAssignment(Tree<Token>* root, FILE* output)    // returns value in ax ...  // ???
{
    assert(root);
    assert(output);

    // Only lvalue (variable) can be to the left of '='
    if(root->Left()->Data().Type() != VARIABLE_TO_USE)
    {
        std::cout << "Rvalue as left operand of assignment: '" << root->Left()->Data().Name() << "'\n";
        return -ERROR::INVALID_ARG;
    }

    // Here everything is ok. Printing right side
    PrintExpression(root->Right(), output);

    // Value to be assigned is in ax now. Do not touch it. Getting variable
    int variable_offset = root->Left()->Data().Value();

    // Check if it is parameter or not
    if(variable_offset < 0)
    {
        fprintf(output, "mov  cx , %d\n", -variable_offset + 1);
        fprintf(output, "mov  bx , bp\n");
        fprintf(output, "add  bx , cx\n");
    }
    else
    {
        fprintf(output, "mov  cx , %d\n",  variable_offset + 1);
        fprintf(output, "mov  bx , bp\n");
        fprintf(output, "sub  bx , cx\n");
    }

    // Assignment itself
    fprintf(output, "mov  [ bx ] , ax\n");

    return ERROR::OK;
}

int PrintOperator(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    int status = 0;

    if(root->Data().Type() == OPERATOR)
    {
        switch (root->Data().Value())
        {
            case IF_OPERATOR:
            {
                PrintIf                 (root, output);
                break;
            }
            case UNTIL_OPERATOR:
            {
                PrintUntil              (root, output);
                break;
            }
            case VARIABLE_TO_CREATE:
            {
                PrintVariableToCreate   (root, output);
                break;
            }
            case RETURN_OPERATOR:
            {
                PrintReturn             (root, output);
                break;
            }
            case ASM_INSERT:
            {
                PrintAsmInsert          (root, output);
                break;
            }
            case COMPLEX_OPERATOR:
            {
                PrintComplex            (root, output);
                break;
            }
            case ASSIGNMENT_OPERATOR:
            {
                status = PrintAssignment(root, output);
                break;
            }
        }
    }
    else    // Just expression
    {
        PrintExpression                 (root, output);
    }

    return status;
}

int PrintVariable(Tree<Token>* root, FILE* output)      // ???
{
    assert(root);
    assert(output);

    int variable_offset = root->Data().Value();
    // Check if variable is argument or inner variable
    if(variable_offset < 0)
    {
        fprintf(output, "mov  ax , %d\n", -variable_offset + 1);
        fprintf(output, "mov  bx , bp\n");
        fprintf(output, "add  bx , ax\n");
    }
    else
    {
        fprintf(output, "mov  ax , %d\n",  variable_offset + 1);
        fprintf(output, "mov  bx , bp\n");
        fprintf(output, "sub  bx , ax\n");
    }

    fprintf(output, "mov  ax , [ bx ]\n");

    return ERROR::OK;
}

int PrintFunctionCall(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    // First push arguments
    int arguments_pushed = 0;
    Tree<Token>* current_argument = root->Right();
    while(current_argument)
    {
        if(current_argument->Left())
        {
            PrintExpression(current_argument->Left(), output);
            fprintf(output, "push ax\n");

            arguments_pushed++;
        }
        current_argument = current_argument->Right();
    }

    // Calling function itself
    fprintf(output, "call $%s\n", root->Data().Name().c_str());

    // Popping arguments NOT TO AX
    for(int i = 0; i < arguments_pushed; i++)
    {
        fprintf(output, "pop  cx\n");
    }

    return ERROR::OK;
}

int PrintConstant(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    fprintf(output, "mov  ax , %d\n", root->Data().Value());

    return ERROR::OK;
}

int PrintExpression(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    switch(root->Data().Type())
    {
        case BIN_OPERATION:
        {
            PrintBinOperator    (root, output);
            break;
        }
        case VARIABLE_TO_USE:
        {
            PrintVariable       (root, output);
            break;
        }
        case FUNCTION_TO_USE:
        {
            PrintFunctionCall   (root, output);
            break;
        }
        case CONSTANT:
        {
            PrintConstant       (root, output);
            break;
        }
        default:
        {
            std::cout << "Unexpected node in PrintExpression(): pointer = " << (void*)root << "\n";
            return -ERROR::INVALID_ARG;
        }
    }

    return ERROR::OK;
}

int PrintBinOperator(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    if(root->Data().Type() != BIN_OPERATION)
    {
        std::cout << "Unexpected node in PrintBinOperator(): pointer = " << (void*)root << "\n";
        return -ERROR::INVALID_ARG;
    }

    int label_num = different_label_names++;

    // Printing right node
    PrintExpression(root->Right(), output);

    // Saving to the stack
    fprintf(output, "push ax\n");

    // Printing left  node
    PrintExpression(root->Left(),  output);

    // Placing second argument to bx
    fprintf(output, "pop  bx\n");

    // Identifying action type
    switch(root->Data().Value())
    {
        // GetE()
        case '+':
        {
            fprintf(output, "add  ax , bx\n");
            break;
        }
        case '-':
        {
            fprintf(output, "sub  ax , bx\n");
            break;
        }
        case '>':
        {
            fprintf(output, "cmp  ax , bx\n");
            fprintf(output, "ja   $above_%d\n",   label_num);
            fprintf(output, "mov  ax , 0 \n");
            fprintf(output, "jmp  $cmp_end_%d\n", label_num);

            fprintf(output, "%%above_%d\n",       label_num);
            fprintf(output, "mov  ax , 1 \n");

            fprintf(output, "%%cmp_end_%d\n",     label_num);
            break;
        }
        case '<':
        {
            fprintf(output, "cmp  ax , bx\n");
            fprintf(output, "jb   $below_%d\n",   label_num);
            fprintf(output, "mov  ax , 0 \n");
            fprintf(output, "jmp  $cmp_end_%d\n", label_num);

            fprintf(output, "%%below_%d\n",       label_num);
            fprintf(output, "mov  ax , 1 \n");

            fprintf(output, "%%cmp_end_%d\n",     label_num);
            break;
        }
        case '~':
        {
            fprintf(output, "cmp  ax , bx\n");
            fprintf(output, "je   $equal_%d\n",   label_num);
            fprintf(output, "mov  ax , 0 \n");
            fprintf(output, "jmp  $cmp_end_%d\n", label_num);

            fprintf(output, "%%equal_%d\n",       label_num);
            fprintf(output, "mov  ax , 1 \n");

            fprintf(output, "%%cmp_end_%d\n",     label_num);
            break;
        }
        // GetT()
        case '*':
        {
            fprintf(output, "mul  bx\n");
            break;
        }
        case '/':
        {
            fprintf(output, "div  bx\n");
            break;
        }
    }

    return ERROR::OK;
}

int PrintFunctionDeclare(Tree<Token>* root, FILE* output)
{
    assert(root);
    assert(output);

    // Printing label
    fprintf(output, "%%%s\n", root->Data().Name().c_str());
    fprintf(output, "push bp\n");
    fprintf(output, "mov  bp , sp\n\n");

    // Printing main part, probably...
    PrintOperator(root->Left(), output);

    // Returning
    fprintf(output, "\nmov  sp , bp\n");
    fprintf(output, "pop  bp\n");
    fprintf(output, "ret\n\n");

    return ERROR::OK;
}

int PrintGlobal(Tree<Token>* root, FILE* output)
{
    // function called "main" must present
    fprintf(output, "call $main\n");
    fprintf(output, "end\n\n");

    while(root)
    {
        if(PrintFunctionDeclare(root->Left(), output))  return -ERROR::UNKNOWN;
        root = root->Right();
    }
    return ERROR::OK;
}





