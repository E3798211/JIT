#include "JIT.h"

/// Global varaible to contain current position in raw buffer
int current_raw_command  = 0;

/// Global varaible to contain current position in brepared buffer
int current_prep_command = 0;

// Both of them are changed in functions that produce transforming




int JitCompile(const std::string byte_code_filename)
{
    int raw_executable_bytes[MAX_PROGRAM_SIZE] = {};
/*
    FILE* input = fopen(byte_code_filename.c_str(), "r");
    if(!input)
    {
        std::cout << "Failed to open '" << byte_code_filename << "'\n";
        return ERROR::FILE_NOT_OPENED;
    }
*/

    // Reading file, filling raw_exec_bytes with ints
//    int n_raw_bytes = ReadRawBytes(raw_executable_bytes, input);
/*
    fclose(input);
    input = nullptr;
*/
    // Allocating memory with mmap

    char* programm = (char*)mmap(   NULL, MAX_PROGRAM_SIZE, PROT_WRITE | PROT_EXEC,
                             MAP_ANON | MAP_PRIVATE, -1, 0);
    if(!programm)
    {
        std::cout << "Failed to allocate memory for programm buffer. JIT compilation failed\n";
        return -ERROR::UNKNOWN;
    }

    // Preparing program in x86 opcodes
/*
    raw_executable_bytes[current_raw_command++] = 0;
    raw_executable_bytes[current_raw_command++] = 0;
    raw_executable_bytes[current_raw_command++] = 0x1234;   // 1

    raw_executable_bytes[current_raw_command++] = ADD;
    raw_executable_bytes[current_raw_command++] = BX;
    raw_executable_bytes[current_raw_command++] = AX;
    raw_executable_bytes[current_raw_command++] = ADD;
    raw_executable_bytes[current_raw_command++] = AX;
    raw_executable_bytes[current_raw_command++] = BX;
    raw_executable_bytes[current_raw_command++] = ADD;
    raw_executable_bytes[current_raw_command++] = BX;
    raw_executable_bytes[current_raw_command++] = CX;       // 4

    raw_executable_bytes[current_raw_command++] = SUB;
    raw_executable_bytes[current_raw_command++] = AX;
    raw_executable_bytes[current_raw_command++] = BX;
    raw_executable_bytes[current_raw_command++] = SUB;
    raw_executable_bytes[current_raw_command++] = BX;
    raw_executable_bytes[current_raw_command++] = AX;
    raw_executable_bytes[current_raw_command++] = SUB;
    raw_executable_bytes[current_raw_command++] = BX;
    raw_executable_bytes[current_raw_command++] = CX;
    raw_executable_bytes[current_raw_command++] = SUB;
    raw_executable_bytes[current_raw_command++] = SP;
    raw_executable_bytes[current_raw_command++] = CX;       // 8

    raw_executable_bytes[current_raw_command++] = DIV;
    raw_executable_bytes[current_raw_command++] = BX;
    raw_executable_bytes[current_raw_command++] = MUL;
    raw_executable_bytes[current_raw_command++] = BX;       // 10

    current_raw_command = 0;


//    BinTox86(raw_executable_bytes, n_raw_bytes, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
    BinTox86(raw_executable_bytes, MAX_PROGRAM_SIZE, programm);
*/
    programm[current_prep_command++] = '\xc3';
    // Calling this function

    std::cout << "Calling\n";

    ((void (*)())programm)();

    return ERROR::OK;
}

int ReadRawBytes(int buffer[], FILE* input)
{
    assert(input);

    int i = 0;
    while(fscanf(input, "%d", buffer + i) != EOF)     i++;

    return i;
}

int BinTox86(int buffer[], int n_raw_bytes, char programm[])
{
    // Giant switch


    /* */if(buffer[current_raw_command] >= MOV_REG_NUM &&
            buffer[current_raw_command] <= MOV_RAM_REG_REG)
    {
        std::cout << "Mov\n";
        TransformMov(buffer, programm);
    }
    else if(buffer[current_raw_command] >= PUSH &&
            buffer[current_raw_command] <= POP)
    {
        std::cout << "Push|Pop\n";
        TransformPushPop(buffer, programm);
    }
    else if(buffer[current_raw_command] >= ADD  &&
            buffer[current_raw_command] <= MUL)
    {
        std::cout << "Arithmetics\n";
        TransformArithmetics(buffer, programm);
    }
    else if(buffer[current_raw_command] >= JMP  &&
            buffer[current_raw_command] <= CMP)
    {
        // Later...
    }
    else if(buffer[current_raw_command] >= CALL &&
            buffer[current_raw_command] <= RET)
    {
        // Later...
    }
    else if(buffer[current_raw_command] >= IN   &&
            buffer[current_raw_command] <= OUT)
    {
        // Later...
    }
    else if(buffer[current_raw_command] == NOP)
    {
        // Nothing :)
    }
    else if(buffer[current_raw_command] == END)
    {
        // Placing 'ret'
        programm[current_prep_command++] = '\xc3';
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

            // Placing num
            PlaceNumToProgramm(programm, num_to_be_assigned);

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
                    programm[current_prep_command++] = '\xd8';
                    break;
                case BX:
                {
                    if(src_reg_num == AX)
                    {
                        programm[current_prep_command++] = '\xc3';
                    }
                    else
                    {
                        programm[current_prep_command++] = '\xcb';
                    }
                    break;
                }
                case SP:
                    programm[current_prep_command++] = '\xcc';
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

            current_raw_command--;  // As it will be incremented lately

            break;
        }
        case MUL:
        {
            // Only one situation possible

            programm[current_prep_command++] = '\x48';
            programm[current_prep_command++] = '\xf7';
            programm[current_prep_command++] = '\xe3';

            current_raw_command--;  // As it will be incremented lately

            break;
        }
    }

    // Skipping arguments
    current_raw_command += 3;

    return ERROR::OK;
}

int TransformJmp(int buffer[], char programm[])
{
    //
}





inline int PlaceNumToProgramm(char programm[], int num)
{
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 4)) & 0xFF);
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 3)) & 0xFF);
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 2)) & 0xFF);
    programm[current_prep_command++] = (char)((num >> BYTE_SIZE * (sizeof(int) - 1)) & 0xFF);

    return ERROR::OK;
}




