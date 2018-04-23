#ifndef VIRTUALASM_H_INCLUDED
#define VIRTUALASM_H_INCLUDED

#include <iostream>
#include <string>
#include <ctype.h>

#include "../CommonFiles/Errors.h"
#include "../../Service/Service.h"
#include "../CommonFiles/CPUProperties.h"
#include "../CommonFiles/Commands.h"

#define ASM_DBG_
#ifdef ASM_DBG_
    #define ASM_DBG
#else
    #define ASM_DBG if(0)
#endif // ASM_DBG_


struct Label
{
    /// name
    std::string name_;

    /// address
    int address_ = 0;
};

const char DEFAULT_ASM_OUTPUT_FILENAME[] = "AsmOut";

/// Creates file with opcodes
int Assembler(std::string asm_file_name);

/// Writes commands to the output file
/**
    \return Amount of commands printed
*/
int LoadOutputFile(FILE* output, int* cmds, size_t n_cmds);

/// Checks if there are any negative numbers
bool FinalProgrammIsOk(int* cmds, size_t n_cmds);

/// Gets line
/**
    Line means a part of a string ended by a '\n'

    \param [out] beg    Pointer to the string to be parsed.
    \param [out] end    Pointer to the end of the line. Is set to the next charecter after end.
*/
char* GetLine(char** beg, char** end);

/// Assembles line and writes to the output file
int AssembleLine(   int*& cmds, size_t& cur_cmd, char* beg, char* end,
                    Label* labels, size_t* n_labels);

/// Looks for label in array of labels
/**
    \return Number on the label or (-V_ERR:V_NOT_FOUND)
*/
int FindLabel(std::string name, Label* labels, size_t n_labels);

/// Gets Mov command
/**
    \warning Changes all arguments
*/
int Mov(int*& cmds, size_t& cur_cmd, char*& beg, char*& end);

/// Gets Jump command
int Jump(   int*& cmds, size_t& cur_cmd, char*& beg, char*& end,
            Label* labels, size_t n_labels, int jmp);

/// Gets register num
int GetRegNum(std::string reg_name);

/// Gets word
/**
    \warning Stops pointer on the next character after word
*/
std::string GetWord(char** beg);

#endif // VIRTUALASM_H_INCLUDED
