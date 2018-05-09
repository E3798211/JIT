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


const int MAX_SHORT_JUMP_FORWARD  =  128;
const int MAX_SHORT_JUMP_BACKWARD = -127;

const char OUT_REPLACEMENT[]    =

        "\x4c\x8b\x45\x10"
        "\x4d\x31\xc9"
        "\x41\xba\x0a\x00\x00\x00"
        "\xbe\xfe\x00\x40\x00"
        "\x48\x8d\x35\x69\x00\x00\x00"
        "\x4c\x89\xc0"
        "\x48\x31\xd2"
        "\x49\xf7\xf2"
        "\x49\x89\xc0"
        "\x48\x83\xc2\x30"
        "\x88\x16"
        "\x48\xff\xce"
        "\x49\xff\xc1"
        "\x49\x83\xf8\x00"
        "\x75\xe3"
        "\x48\xff\xc6"
        "\x4c\x89\xca"

        "\xb8\x01\x00\x00\x00"
        "\xbf\x01\x00\x00\x00"
        "\x0f\x05"
        "\xeb\x40"
        "\x0a\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
        "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
        "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
        "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x10"

        "\x48\x8d\x35\xb9\xff\xff\xff"
        "\xba\x01\x00\x00\x00"
        "\xb8\x01\x00\x00\x00"
        "\xbf\x01\x00\x00\x00"
        "\x0f\x05"

        "\xc3";

const int OUT_BEGIN_ADDR  = MAX_PROGRAM_SIZE;

const int LIBRARY_BEGIN   = OUT_BEGIN_ADDR;
const int LIBRARY_LENGTH  = sizeof(OUT_BEGIN_ADDR);    // For now

/// Transforms bin file to x86 opcodes and executes it
/**
    \param [in] byte_code_filename Name of the file with byte commands for virtual machine
*/
int JitCompile(const std::string byte_code_filename);

/// Reads file and fills buffer with codes
/**
    \return Amount of opcodes read
*/
int ReadRawBytes        (int buffer[], FILE* input);

/// Inits program
int InitProgramm        (int raw_commands[], char programm[], int n_commands);

/// Transforms bytes for out machine to x86 opcodes
int BinTox86            (int buffer[], char programm[]);

/// Transform mov
int TransformMov        (int buffer[], char programm[]);

/// Transform Stack operations
int TransformPushPop    (int buffer[], char programm[]);

/// Transform Arithmetics
int TransformArithmetics(int buffer[], char programm[]);

/// Transform Jumps
/**
    Relative position is oriented on the end of a command (beginning of the next one actually)
*/
int TransformJmp        (int buffer[], char programm[]);

/// Transforms 'call' and 'ret' commands
int TransformCallRet    (int buffer[], char programm[]);

/// Transforms 'end' command
int TransformEnd        (int buffer[], char programm[]);

/// Transforms 'cmp' command
int TransformCmp        (int buffer[], char programm[]);

/// Transforms 'nop' command
int TransformNop        (int buffer[], char programm[]);

/// Transforms 'in' and 'out' commands
int TransformInOut      (int buffer[], char programm[]);

/// Places num in a correct way to programm
inline int PlaceNumToProgramm   (char programm[], int num);

/// Corrects num if it is necessary
/**
    \return Corrected num
*/
inline int CorrectNum           (int buffer[], int num);


#endif // JIT_H_INCLUDED
