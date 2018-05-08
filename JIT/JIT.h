#ifndef JIT_H_INCLUDED
#define JIT_H_INCLUDED

#include <iostream>
#include <string>
#include <assert.h>

// For mmap
#include <sys/mman.h>
#include <arpa/inet.h>

#include "../Service/Errors.h"
#include "../VirtualProc/CommonFiles/CPUProperties.h"
#include "../VirtualProc/CommonFiles/Commands.h"
#include "x86opcodes.h"


const int MAX_SHORT_JUMP_FORWARD  =  127;
const int MAX_SHORT_JUMP_BACKWARD = -127;


/// Transforms bin file to x86 opcodes and executes it
/**
    \param [in] byte_code_filename Name of the file with byte commands for virtual machine
*/
int JitCompile(const std::string byte_code_filename);

/// Reads file and fills buffer with codes
/**
    \return Amount of opcodes read
*/
int ReadRawBytes(int buffer[], FILE* input);

/// Transforms bytes for out machine to x86 opcodes
int BinTox86(int buffer[], int n_raw_bytes, char programm[]);

/// Transform mov
int TransformMov(int buffer[], char programm[]);

/// Transform Stack operations
int TransformPushPop(int buffer[], char programm[]);

/// Transform Arithmetics
int TransformArithmetics(int buffer[], char programm[]);

/// Transform Jumps
/**
    Relative position is oriented on the end of a command (beginning of the next one actually)
*/
int TransformJmp(int buffer[], char programm[]);

/// Places num in a correct way to programm
inline int PlaceNumToProgramm(char programm[], int num);


#endif // JIT_H_INCLUDED
