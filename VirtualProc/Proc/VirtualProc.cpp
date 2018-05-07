#include "VirtualProc.h"

int VirtualProc::LoadFile(std::string filename)
{
    char* file_content = FileRead(filename);
    if(!file_content)
    {
        PROC_DBG std::cout << "Failed to load '" << filename << "'\n";
        return -ERROR::LOAD_ERR;
    }

    size_t prog_len = CountWords(file_content);

    /*
     *  Two first words are version and signature. Can be added lately.
     */

    if(prog_len >= MAX_PROGRAM_SIZE)
    {
        PROC_DBG std::cout << "Programm size is too big. Cannot be loaded\n";
        delete [] file_content;
        return -ERROR::LOAD_ERR;
    }

    char* file_content_iterator = file_content;
    for(size_t i = 0; i < prog_len; i++)
        program_[i] = strtol(file_content_iterator, &file_content_iterator, 10);

    delete [] file_content;
    return ERROR::OK;
}

int VirtualProc::Exec()
{
    switch (program_[registers_[IP]])
    {
        case MOV_REG_NUM:
        {
            int dst_reg_num = program_[registers_[IP] + 1];
            int         num = program_[registers_[IP] + 2];
            registers_[dst_reg_num] = num;

            registers_[IP] += 2;
            break;
        }
        case MOV_REG_RAM_REG:
        {
            int dst_reg_num = program_[registers_[IP] + 1];
            int src_reg_num = program_[registers_[IP] + 2];
            registers_[dst_reg_num] = ram_[src_reg_num];

            registers_[IP] += 2;
            break;
        }
        case MOV_REG_REG:
        {
            int dst_reg_num = program_[registers_[IP] + 1];
            int src_reg_num = program_[registers_[IP] + 2];
            registers_[dst_reg_num] = registers_[src_reg_num];

            registers_[IP] += 2;
            break;
        }
        case MOV_RAM_REG_REG:
        {
            int dst_reg_num = program_[registers_[IP] + 1];
            int src_reg_num = program_[registers_[IP] + 2];
            ram_[dst_reg_num] = registers_[src_reg_num];

            registers_[IP] += 2;
            break;
        }
        // =================================
        case PUSH:
        {
            int src_reg_num = program_[registers_[IP] + 1];
            registers_[SP] -= 1;
            // ram_[STACK_BEGIN + registers_[SP]] = registers_[src_reg_num];
            ram_[registers_[SP]] = registers_[src_reg_num];

            registers_[IP] += 1;
            break;
        }
        case POP:
        {
            if(registers_[SP] < 0)
            {
                std::cout << "Stack is empty, SP = -1, cannot got further\n";
                return -ERROR::UNDERFLOW;
            }

            int dst_reg_num = program_[registers_[IP] + 1];
            // registers_[dst_reg_num] = ram_[STACK_BEGIN + registers_[SP]];
            registers_[dst_reg_num] = ram_[registers_[SP]];

            registers_[SP] += 1;
            registers_[IP] += 1;
            break;
        }
        // =================================
        case ADD:
        {
            int dst_reg_num = program_[registers_[IP] + 1];
            int src_reg_num = program_[registers_[IP] + 2];

            registers_[dst_reg_num] = registers_[dst_reg_num] + registers_[src_reg_num];
            registers_[IP] += 2;
            break;
        }
        case SUB:
        {
            int dst_reg_num = program_[registers_[IP] + 1];
            int src_reg_num = program_[registers_[IP] + 2];

            registers_[dst_reg_num] = registers_[dst_reg_num] - registers_[src_reg_num];
            registers_[IP] += 2;
            break;
        }
        case DIV:
        {
            int src_reg_num = program_[registers_[IP] + 1];
            registers_[AX] = registers_[AX]/registers_[src_reg_num];

            registers_[IP] += 1;
            break;
        }
        case MUL:
        {
            int src_reg_num = program_[registers_[IP] + 1];
            registers_[AX] = registers_[AX]*registers_[src_reg_num];

            registers_[IP] += 1;
            break;
        }
        // =================================
        case JMP:
        {
            int landing_place = program_[registers_[IP] + 1];
            registers_[IP] = landing_place;

            registers_[IP] -= 1;    // As it will be incremented in the end of function
            break;
        }
        case JE:
        {
            JumpCode([] (int a){ return a == EQUAL; });
            break;
        }
        case JNE:
        {
            JumpCode([] (int a){ return a != EQUAL; });
            break;
        }
        case JA:
        {
            JumpCode([] (int a){ return a >  EQUAL; });
            break;
        }
        case JAE:
        {
            JumpCode([] (int a){ return a >= EQUAL; });
            break;
        }
        case JB:
        {
            JumpCode([] (int a){ return a <  EQUAL; });
            break;
        }
        case JBE:
        {
            JumpCode([] (int a){ return a <= EQUAL; });
            break;
        }
        case CMP:
        {
            int dst_reg_num = program_[registers_[IP] + 1];
            int src_reg_num = program_[registers_[IP] + 2];
            registers_[CMP_FLAG] = registers_[dst_reg_num] - registers_[src_reg_num];

            registers_[IP] += 2;
            break;
        }
        // =================================
        case CALL:
        {
            int landing_place = program_[registers_[IP] + 1];
            int  return_place = registers_[IP] + 2;

            registers_[IP]  = landing_place;
            registers_[SP]  -= 1;
            // ram_[STACK_BEGIN + registers_[SP]] = return_place;
            ram_[registers_[SP]] = return_place;


            registers_[IP] -= 1;    // As it will be incremented in the end of function
            break;
        }
        case RET:
        {
            // int return_place = ram_[STACK_BEGIN + registers_[SP]];
            int return_place = ram_[registers_[SP]];
            registers_[SP]  += 1;
            registers_[IP]   = return_place;

            registers_[IP] -= 1;    // As it will be incremented in the end of function
            break;
        }
        case NOP:
        {
            break;
        }
        case IN:
        {
            int in_value = 0;
            std::cout << " << ";
            std::cin >> in_value;

            registers_[SP] -= 1;
            // ram_[STACK_BEGIN + registers_[SP]] = in_value;
            ram_[registers_[SP]] = in_value;

            break;
        }
        case OUT:
        {
            if(registers_[SP] < 0)
                std::cout << " >> 0\n";
            else
            {
                std::cout << " >> " << ram_[STACK_BEGIN - registers_[SP]] << "\n";
                // std::cout << " >> " << ram_[STACK_BEGIN + registers_[SP]] << "\n";
                std::cout << " >> " << registers_[SP] << "\n";
            }
            break;
        }
    }

    registers_[IP]++;

    return ERROR::OK;
}

int VirtualProc::ShowInfo()
{
    std::cout << "Registers:\n";
    for(int i = 0; i < N_REGS; i++)
    {
        std::cout << "reg " << i << "\t= " << registers_[i] << "\n";
    }
    std::cout << "Ram:\n";
    for(int i = 0; i < 10; i++)
    {
        std::cout << "ram [" << i << "]\t= " << ram_[i] << "\n";
    }
    std::cout << "Stack:\n";
    for(int i = STACK_BEGIN; i > STACK_BEGIN - 10; i--)
    {
        std::cout << "stck[" << i << "]\t= " << ram_[i] << "\n";
    }

    return ERROR::OK;
}

int VirtualProc::JumpCode(bool (*check)(int flag_value))
{
    if(check(registers_[CMP_FLAG]))
    {
        int landing_place = program_[registers_[IP] + 1];
        registers_[IP] = landing_place;

        registers_[IP] -= 1;    // As it will be incremented in the end of function
    }
    else
    {
        registers_[IP] += 1;    // Skipping landing label
    }

    return ERROR::OK;
}




