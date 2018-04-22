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
    int address = 0;
};

const char DEFAULT_ASM_OUTPUT_FILENAME[] = "AsmOut";

/// Creates file with opcodes
int Assembler(std::string asm_file_name);

/// Gets line
/**
    Line means a part of a string ended by a '\n'

    \param [out] beg    Pointer to the string to be parsed.
    \param [out] end    Pointer to the end of the line. Is set to the next charecter after end.
*/
char* GetLine(char** beg, char** end);

/// Assembles line and writes to the output file
int AssembleLine(char*& cmds, size_t& cur_cmd, char* beg, char* end);

/// Gets register num
int GetRegNum(std::string reg_name);

/// Gets word
std::string GetWord(char** beg);

#endif // VIRTUALASM_H_INCLUDED
