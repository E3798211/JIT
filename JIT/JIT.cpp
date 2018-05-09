#include "JIT.h"

/// Global varaible to contain current position in raw buffer
int current_raw_command  = 0;

/// Global varaible to contain current position in prepared buffer
int current_prep_command = 0;

// Both of them are changed in functions that produce transforming




int JitCompile(const std::string byte_code_filename)
{
    int raw_executable_bytes[MAX_PROGRAM_SIZE] = {};

    FILE* input = fopen(byte_code_filename.c_str(), "r");
    if(!input)
    {
        std::cout << "Failed to open '" << byte_code_filename << "'\n";
        return ERROR::FILE_NOT_OPENED;
    }

    // Reading file, filling raw_exec_bytes with ints
    int n_raw_bytes = ReadRawBytes(raw_executable_bytes, input);

    fclose(input);
    input = nullptr;

    // Allocating memory with mmap

    char* programm = (char*)mmap(   NULL, MAX_PROGRAM_SIZE + sizeof(OUT_REPLACEMENT) + 10, PROT_WRITE | PROT_EXEC,
                             MAP_ANON | MAP_PRIVATE, -1, 0);
    if(!programm)
    {
        std::cout << "Failed to allocate memory for programm buffer. JIT compilation failed\n";
        return -ERROR::UNKNOWN;
    }

    // Preparing program in x86 opcodes

    while(current_raw_command < n_raw_bytes)
    {
        BinTox86(raw_executable_bytes, programm);
    }

    int library_end = MAX_PROGRAM_SIZE + sizeof(OUT_REPLACEMENT) + 1;
    for(int i = MAX_PROGRAM_SIZE; i < library_end; i++)
        programm[i] = OUT_REPLACEMENT[i - MAX_PROGRAM_SIZE];

    // Calling this function

    asm volatile
    (
        "mov  %rbp, %r15\n"
        "mov  %rsp, %r14\n"
    );
    ((void (*)())programm)();
    asm volatile
    (
        "mov  %r15, %rbp\n"
        "mov  %r14, %rsp\n"
    );

    // delete [] programm;

    return ERROR::OK;
}

int ReadRawBytes(int buffer[], FILE* input)
{
    assert(input);

    int i = 0;
    while(fscanf(input, "%d", buffer + i) != EOF)     i++;

    return i;
}

int InitProgramm(int raw_commands[], char programm[], int n_commands)
{
    // Creating programm

    while(current_raw_command < n_commands)
    {
        BinTox86(raw_commands, programm);
    }

    // Adding library

    int library_end = LIBRARY_BEGIN + LIBRARY_LENGTH + 1;
    for(int i = LIBRARY_BEGIN; i < library_end; i++)
        programm[i] = OUT_REPLACEMENT[i - MAX_PROGRAM_SIZE];

    return ERROR::OK;
}

int BinTox86(int buffer[], char programm[])
{
    // Giant switch


    /* */if(buffer[current_raw_command] >= MOV_REG_NUM &&
            buffer[current_raw_command] <= MOV_RAM_REG_REG)
    {
        TransformMov        (buffer, programm);
    }
    else if(buffer[current_raw_command] >= PUSH &&
            buffer[current_raw_command] <= POP)
    {
        TransformPushPop    (buffer, programm);
    }
    else if(buffer[current_raw_command] >= ADD  &&
            buffer[current_raw_command] <= MUL)
    {
        TransformArithmetics(buffer, programm);
    }
    else if(buffer[current_raw_command] >= JMP  &&
            buffer[current_raw_command] <= JBE)
    {
        TransformJmp        (buffer, programm);
    }
    else if(buffer[current_raw_command] == CMP)
    {
        TransformCmp        (buffer, programm);
    }
    else if(buffer[current_raw_command] >= CALL &&
            buffer[current_raw_command] <= RET)
    {
        TransformCallRet    (buffer, programm);
    }
    else if(buffer[current_raw_command] >= IN   &&
            buffer[current_raw_command] <= OUT)
    {
        TransformInOut      (buffer, programm);
    }
    else if(buffer[current_raw_command] == NOP)
    {
        TransformNop        (buffer, programm);
    }
    else if(buffer[current_raw_command] == END)
    {
        TransformEnd        (buffer, programm);
    }
    else
    {
        std::cout << "Unknown byte command: " << buffer[current_raw_command] << "\n";
        return -ERROR::INVALID_ARG;
    }

    return -ERROR::OK;
}

int TransformMov(int buffer[], char programm[])
{
    // Four possible situations
    switch(buffer[current_raw_command])
    {
        case MOV_REG_NUM:
        {
            // Getting register and number
            int dst_reg_num         = buffer[current_raw_command + 1];
            int num_to_be_assigned  = buffer[current_raw_command + 2];

            // Placing first bytes
            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\xc7';

            // Placing register's num
            switch(dst_reg_num)
            {
                case AX:
                    programm[current_prep_command++] = '\xc0';
                    break;
                case BX:
                    programm[current_prep_command++] = '\xc3';
                    break;
                case CX:
                    programm[current_prep_command++] = '\xc1';
                    break;
                case BP:
                    programm[current_prep_command++] = '\xc5';
                    break;
                case SP:
                    programm[current_prep_command++] = '\xc4';
                    break;
            }

            // Correct num?
            num_to_be_assigned = CorrectNum(buffer, num_to_be_assigned);

            // Placing num
            PlaceNumToProgramm(programm, num_to_be_assigned);

            // Aligning
            current_raw_command += 4;   // Skipping 4 nops, which are created only for VirtualMachine

            break;
        }
        case MOV_REG_RAM_REG:
        {
            // Now compiler needs only one one such instruction

            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\x8b';
            programm[current_prep_command++] = '\x03';

            break;
        }
        case MOV_REG_REG:
        {
            // Getting register and number
            int dst_reg_num = buffer[current_raw_command + 1];
            int src_reg_num = buffer[current_raw_command + 2];

            // Placing first bytes
            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\x89';

            // Placing register's num
            switch(dst_reg_num)
            {
                case BP:
                    programm[current_prep_command++] = '\xe5';
                    break;
                case SP:
                    programm[current_prep_command++] = '\xec';
                    break;
                case BX:
                {
                    if(src_reg_num == BP)
                    {
                        programm[current_prep_command++] = '\xeb';
                    }
                    else
                    {
                        programm[current_prep_command++] = '\xc3';
                    }
                    break;
                }
            }

            break;
        }
        case MOV_RAM_REG_REG:
        {
            // Now compiler needs only one one such instruction

            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\x89';
            programm[current_prep_command++] = '\x03';

            break;
        }
    }

    // Skipping arguments
    current_raw_command += 3;

    return ERROR::OK;
}

int TransformPushPop(int buffer[], char programm[])
{
    int reg_num = buffer[current_raw_command + 1];

    switch(buffer[current_raw_command])
    {
        case PUSH:
        {
            switch(reg_num)
            {
                case AX:
                    programm[current_prep_command++] = '\x50';
                    break;
                case BP:
                    programm[current_prep_command++] = '\x55';
                    break;
            }
            break;
        }
        case POP:
        {
            switch(reg_num)
            {
                case AX:
                    programm[current_prep_command++] = '\x58';
                    break;
                case BX:
                    programm[current_prep_command++] = '\x5b';
                    break;
                case CX:
                    programm[current_prep_command++] = '\x59';
                    break;
                case BP:
                    programm[current_prep_command++] = '\x5d';
                    break;
            }
            break;
        }
    }

    // ALIGNMENT
    programm[current_prep_command++] = '\x90';

    // Skipping arguments
    current_raw_command += 2;

    return ERROR::OK;
}

int TransformArithmetics(int buffer[], char programm[])
{
    switch(buffer[current_raw_command])
    {
        case ADD:
        {
            // Getting register and number
            int dst_reg_num = buffer[current_raw_command + 1];
            int src_reg_num = buffer[current_raw_command + 2];

            // Placing first bytes
            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\x01';

            switch(src_reg_num)
            {
                case AX:
                    programm[current_prep_command++] = '\xc3';
                    break;
                case BX:
                    programm[current_prep_command++] = '\xd8';
                    break;
                case CX:
                    programm[current_prep_command++] = '\xcb';
                    break;
            }

            break;
        }
        case SUB:
        {
            // Getting register and number
            int dst_reg_num = buffer[current_raw_command + 1];
            int src_reg_num = buffer[current_raw_command + 2];

            // Placing first bytes
            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\x29';

            switch(dst_reg_num)
            {
                case AX:
                    programm[current_prep_command++] =      '\xd8';
                    break;
                case BX:
                {
                    if(src_reg_num == AX)
                    {
                        programm[current_prep_command++] =  '\xc3';
                    }
                    else
                    {
                        programm[current_prep_command++] =  '\xcb';
                    }
                    break;
                }
                case SP:
                    programm[current_prep_command++] =      '\xcc';
                    break;
            }

            break;
        }
        case DIV:
        {
            // Only one situation possible

            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\xf7';
            programm[current_prep_command++] = '\xf3';

            // current_raw_command--;  // As it will be incremented lately

            break;
        }
        case MUL:
        {
            // Only one situation possible

            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\xf7';
            programm[current_prep_command++] = '\xe3';

            // current_raw_command--;  // As it will be incremented lately

            break;
        }
    }

    // Skipping arguments
    current_raw_command += 3;

    return ERROR::OK;
}

int TransformJmp(int buffer[], char programm[])
{
    // All jumps are aligned to 5 bytes

    int landing_address  = buffer[current_raw_command + 1];
    int relative_address = landing_address - current_raw_command;

    bool jump_short = (relative_address < MAX_SHORT_JUMP_FORWARD    &&
                       relative_address > MAX_SHORT_JUMP_BACKWARD ) ?   true : false;

    // Taking actual code
    if(jump_short)
    {
        // Correcting jump address
        relative_address -= 2;

        // Getting jump type
        switch(buffer[current_raw_command])
        {
            case JMP:
                programm[current_prep_command++] =  '\xeb';
                break;
            case JA:
                programm[current_prep_command++] =  '\x77';
                break;
            case JB:
                programm[current_prep_command++] =  '\x72';
                break;
            case JE:
                programm[current_prep_command++] =  '\x74';
                break;
        }

        // Placing jump
        PlaceNumToProgramm(programm, relative_address);

        // Replacing last 3 bytes with NOPs
        programm[current_prep_command - 3] =        '\x90';
        programm[current_prep_command - 2] =        '\x90';
        programm[current_prep_command - 1] =        '\x90';

        // Aligning command to 6 bytes
        programm[current_prep_command++]   =        '\x90';
    }
    else
    {
        if(buffer[current_raw_command] == JMP)
        {
            // Correcting jump address
            relative_address -= 5;

            // short jmp opcode
            programm[current_prep_command++] =      '\xe9';

            // Placing landing place
            PlaceNumToProgramm(programm, relative_address);

            // Aligning
            programm[current_prep_command++] =      '\x90';
        }
        else
        {
            // Correcting jump address
            relative_address -= 6;

            programm[current_prep_command++] =      '\x0f';

            switch(buffer[current_raw_command])
            {
                case JA:
                    programm[current_prep_command++] = '\x87';
                    break;
                case JB:
                    programm[current_prep_command++] = '\x82';
                    break;
                case JE:
                    programm[current_prep_command++] = '\x84';
                    break;
            }

            // Placing address
            PlaceNumToProgramm(programm, relative_address);
        }
    }

    // Skipping arguments
    current_raw_command += 6;

    return ERROR::OK;
}

int TransformCallRet(int buffer[], char programm[])
{
    int landing_address  = buffer[current_raw_command + 1];
    int relative_address = landing_address - current_raw_command;

    switch(buffer[current_raw_command])
    {
        case CALL:
        {
            // Correcting address
            relative_address -= 5;

            // Call itself
            programm[current_prep_command++] = '\xe8';

            // Function address
            PlaceNumToProgramm(programm, relative_address);

            // Alignment
            programm[current_prep_command++] = '\x90';

            // Skipping arguments
            current_raw_command += 6;

            break;
        }
        case RET:
        {
            programm[current_prep_command++] = '\xc3';

            // Skipping arguments
            current_raw_command += 1;

            break;
        }
    }

    return ERROR::OK;
}

int TransformEnd(int buffer[], char programm[])
{
    // Placing 'ret'
    programm[current_prep_command++] = '\xc3';

    current_raw_command++;

    return ERROR::OK;
}

int TransformCmp(int buffer[], char programm[])
{
    // Only 'cmp ax, bx' supported

    programm[current_prep_command++] = '\x48';
    programm[current_prep_command++] = '\x39';
    programm[current_prep_command++] = '\xd8';

    // Skipping
    current_raw_command += 3;

    return ERROR::OK;
}

int TransformNop(int buffer[], char programm[])
{
    // nop
    programm[current_prep_command++] = '\x90';

    current_raw_command++;

    return ERROR::OK;
}

int TransformInOut        (int buffer[], char programm[])
{
    switch(buffer[current_raw_command])
    {
        case IN:
        {
            // NOT IMPLEMENTED
            break;
        }
        case OUT:
        {
            // Just call of OUT from our library
            int call_address = OUT_BEGIN_ADDR - current_raw_command;

            // Correcting address
            call_address -= 5;

            // Call itself
            programm[current_prep_command++] = '\xe8';

            // Function address
            PlaceNumToProgramm(programm, call_address);

            //Skipping argument
            current_raw_command += 5;

            break;
        }
    }

    return ERROR::OK;
}




inline int PlaceNumToProgramm(char programm[], int num)
{
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 4)) & 0xFF);
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 3)) & 0xFF);
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 2)) & 0xFF);
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 1)) & 0xFF);

    return ERROR::OK;
}

inline int CorrectNum(int buffer[], int num)
{
    /*
     *
     *  We only need corrections when work with memory, i.e. rsp additions and memory addressing
     *
     *  current_raw_command is set to the beginning of the command, i.e. we can just check further instructions
     *  and arguments
     *
     */

    // parameter getting: mov ax, NUM; mov bx, bp; add bx, ax; mov ax, [bx]

    /* */if(buffer[current_raw_command    ]  == MOV_REG_NUM     &&  // mov ax, NUM
            buffer[current_raw_command + 1]  == AX              &&

            buffer[current_raw_command + 7]  == MOV_REG_REG     &&  // mov bx, bp
            buffer[current_raw_command + 8]  == BX              &&
            buffer[current_raw_command + 9]  == BP              &&

            buffer[current_raw_command + 10] == ADD             &&  // add bx, ax
            buffer[current_raw_command + 11] == BX              &&
            buffer[current_raw_command + 12] == AX              &&

            buffer[current_raw_command + 13] == MOV_REG_RAM_REG &&  // mov ax, [bx]
            buffer[current_raw_command + 14] == AX              &&
            buffer[current_raw_command + 15] == BX
            )
    {
        // As we're addressing to the stack in 64-bit
        return num * 8;
    }

    // Creating variable

    else if(buffer[current_raw_command    ]  == MOV_REG_NUM     &&  // mov cx, 1
            buffer[current_raw_command + 1]  == CX              &&

            buffer[current_raw_command + 7]  == SUB             &&
            buffer[current_raw_command + 8]  == SP              &&
            buffer[current_raw_command + 9]  == CX
            )
    {
        // As we're addressing to the stack in 64-bit
        return num * 8;
    }
    else
    {
        // Not a special situation
        return num;
    }
}


