#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include <iostream>
#include "RecDescend.h"
#include "Syntax.h"

const char COMPILED_CODE_FILENAME[] = "compiled";

/// Compiles code
/**
    Creates file named 'compiled' with asm code
    \param [in] source_code_filename Name of the file to be compiled
*/
int Compile(const std::string source_code_filename);

/// Goes round the syntax tree
/**
    \param [in] syntax_tree Tree built on the base of asm code
*/
int TreeToAsm(Tree<Token>* syntax_tree);


/// Prints If
int PrintIf(Tree<Token>* root, FILE* output);

/// Prints Until
int PrintUntil(Tree<Token>* root, FILE* output);

/// Prints variable-to-create
int PrintVariableToCreate(Tree<Token>* root, FILE* output);

/// Prints Return
int PrintReturn(Tree<Token>* root, FILE* output);

/// Prints asm insert
int PrintAsmInsert(Tree<Token>* root, FILE* output);

/// Prints assignment
int PrintAssignment(Tree<Token>* root, FILE* output);

/// Prints Complex Operator
int PrintComplex(Tree<Token>* root, FILE* output);

/// Prints operator
int PrintOperator(Tree<Token>* root, FILE* output);

/// Prints variable
int PrintVariable(Tree<Token>* root, FILE* output);

/// Prints function call
int PrintFunctionCall(Tree<Token>* root, FILE* output);

/// Prints constant
int PrintConstant(Tree<Token>* root, FILE* output);

/// Prints expressinon
int PrintExpression(Tree<Token>* root, FILE* output);

/// Prints bin operator
int PrintBinOperator(Tree<Token>* root, FILE* output);

/// Prints function declaration nodes
int PrintFunctionDeclare(Tree<Token>* root, FILE* output);

/// Prints global nodes
int PrintGlobal(Tree<Token>* root, FILE* output);


#endif // COMPILER_H_INCLUDED
