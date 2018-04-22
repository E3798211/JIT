#include "VirtualAsm.h"

int Assembler(std::string asm_filename)
{
    char* file_content = FileRead(asm_filename);
    if(!file_content)
    {
        ASM_DBG std::cout << "Failed to load '" << asm_filename << "'\n";
        return -V_ERR::V_LOAD_ERR;
    }

    char cmds[MAX_PROGRAM_SIZE] = {};
    size_t cur_cmd = 0;

    char* line_beg = file_content;
    char* line_end = file_content;
    while(!(*line_end))
    {
        GetLine(&line_beg, &line_end);
        // AssembleLine()
    }

    delete [] file_content;

    FILE* output = fopen(DEFAULT_ASM_OUTPUT_FILENAME, "w");
    fclose(output);

    return V_ERR::V_OK;
}

char* GetLine(char** beg, char** end)
{
    assert(beg);
    assert(end);

    while(isspace(**beg))                       (*beg)++;   // skipping spaces

    *end = *beg;
    while(!isspace(**end) && **end != '\0')     (*end)++;

    return *beg;
}

int AssembleLine(char*& cmds, size_t& cur_cmd, char* beg, char* end)
{
    assert(cmds);
    assert(beg);
    assert(end);

    if(*beg == '#')             // Comment line in assembler
        return V_ERR::V_OK;
    if(*beg == '$')             // Label
        int a;                  // NOT IMPLEMENTED

    std::string cmd = GetWord(&beg);

    /* */if(cmd == MOV_CMD)
    {
        //
    }
    else if(cmd == PUSH_CMD)
    {
        cmds[cur_cmd++] = PUSH;
        cmd = GetWord(&beg);
        int reg_num = GetRegNum(cmd);

        if(reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after push\n";
            return -V_ERR::V_INVALID_ARG;
        }
        else
        {
            cmds[cur_cmd++] = reg_num;
        }
    }
    else if(cmd == POP_CMD)
    {
        cmds[cur_cmd++] = POP;
        cmd = GetWord(&beg);
        int reg_num = GetRegNum(cmd);

        if(reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after push\n";
            return -V_ERR::V_INVALID_ARG;
        }
        else
        {
            cmds[cur_cmd++] = reg_num;
        }
    }
    // =================================
    else if(cmd == ADD_CMD)
    {
        //
    }
    else if(cmd == SUB_CMD)
    {
        //
    }
    else if(cmd == DIV_CMD)
    {
        //
    }
    else if(cmd == MUL_CMD)
    {
        //
    }
    // =================================
    else if(cmd == JMP_CMD)
    {
        //
    }
    else if(cmd == JE_CMD)
    {
        //
    }
    else if(cmd == JNE_CMD)
    {
        //
    }
    else if(cmd == JA_CMD)
    {
        //
    }
    else if(cmd == JAE_CMD)
    {
        //
    }
    else if(cmd == JB_CMD)
    {
        //
    }
    else if(cmd == JBE_CMD)
    {
        //
    }
    // =================================
    else if(cmd == CALL_CMD)
    {
        //
    }
    else if(cmd == RET_CMD)
    {
        cmds[cur_cmd++] = RET;
    }

    while(*beg == ' ')      beg++;
    if(*beg != '\n')
    {
        std::cout << "Extra arguments found. Command number is " << cur_cmd << "\n";
        return -V_ERR::V_INVALID_ARG;
    }
    else
    {
        return V_ERR::V_OK;
    }
}

int GetRegNum(std::string reg_name)
{
    std::string reg_names[] = { AX_REG, BX_REG, CX_REG,  DX_REG,
                                R8_REG, R9_REG, R10_REG, R11_REG,
                                BP_REG, SP_REG };

    #define Check( name )               \
        do{                             \
            if(reg_name == name##_REG)  \
                return name;            \
        }while(0)

    Check(AX);
    Check(BX);
    Check(CX);
    Check(DX);

    Check(R8);
    Check(R9);
    Check(R10);
    Check(R11);

    Check(BP);
    Check(SP);

    #undef Check()

    return -V_ERR::V_NOT_FOUND;
}

std::string GetWord(char** beg)
{
    assert(beg);

    std::string word;
    while(isspace(**beg))    (*beg)++;
    while(!isspace(**beg))
    {
        word += **beg;
        (*beg)++;
    }

    return word;
}


