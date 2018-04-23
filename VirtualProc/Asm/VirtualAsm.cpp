#include "VirtualAsm.h"

int Assembler(std::string asm_filename)
{
    char* file_content = FileRead(asm_filename);
    if(!file_content)
    {
        ASM_DBG std::cout << "Failed to load '" << asm_filename << "'\n";
        return -V_ERR::V_LOAD_ERR;
    }

    Label labels[RAM_SIZE] = {};
    size_t n_labels = 0;

    int cmds[MAX_PROGRAM_SIZE] = {};
    size_t cur_cmd = 0;

    char* line_beg = file_content;
    char* line_end = file_content;

    // First pass

    GetLine(&line_beg, &line_end);
    ASM_DBG std::cout << "Assembling. First pass\n";
    while(*line_end != '\0')
    {
        int* tmp = cmds;
        int status = AssembleLine(tmp, cur_cmd, line_beg, line_end, labels, &n_labels);
        if(status < 0)
        {
            std::cout << "Assembling stopped. Error occured\n";
            cur_cmd = 0;
            break;
        }

        line_beg = line_end;            // Setting to the next line
        GetLine(&line_beg, &line_end);
    }

    // Second pass

    cur_cmd = 0;

    line_beg = file_content;
    line_end = file_content;

    GetLine(&line_beg, &line_end);
    ASM_DBG std::cout << "Assembling. Second pass\n";
    while(*line_end != '\0')
    {
        int* tmp = cmds;
        int status = AssembleLine(tmp, cur_cmd, line_beg, line_end, labels, &n_labels);
        if(status < 0)
        {
            std::cout << "Assembling stopped. Error occured\n";
            cur_cmd = 0;
            break;
        }

        line_beg = line_end;            // Setting to the next line
        GetLine(&line_beg, &line_end);
    }

    delete [] file_content;

    if(!FinalProgrammIsOk(cmds, cur_cmd))
    {
        std::cout << "Invalid (negative) codes in programm\n";
        return -V_ERR::V_INVALID_ARG;
    }

    FILE* output = fopen(DEFAULT_ASM_OUTPUT_FILENAME, "w");
    LoadOutputFile(output, cmds, cur_cmd);
    fclose(output);

    ASM_DBG
    {
        std::cout << "labels:\n";
        for(int i = 0; i < n_labels; i++)
        {
            std::cout << "name = '"     << labels[i].name_ << "'\n";
            std::cout << "address = "   << labels[i].address_ << "\n";
        }
    }

    return V_ERR::V_OK;
}

int LoadOutputFile(FILE* output, int* cmds, size_t n_cmds)
{
    assert(output);
    assert(cmds);

    size_t i = 0;
    for(; i < n_cmds; i ++)
    {
        fprintf(output, "%d\n", cmds[i]);
    }
    fprintf(output, "%d\n", END);

    return i;
}

bool FinalProgrammIsOk(int* cmds, size_t n_cmds)
{
    assert(cmds);

    for(int i = 0; i < n_cmds; i++)
        if(cmds[i] < 0)
            return false;
    return true;
}

char* GetLine(char** beg, char** end)
{
    assert(beg);
    assert(end);

    while(isspace(**beg))                       (*beg)++;   // skipping spaces

    *end = *beg;
    while((**end) != '\n' && **end != '\0')     (*end)++;

    return *beg;
}

int AssembleLine(   int*& cmds, size_t& cur_cmd, char* beg, char* end,
                    Label* labels, size_t* n_labels)
{
    assert(cmds);
    assert(beg);
    assert(end);

    if(*beg == '#')             // Comment line in assembler
        return V_ERR::V_OK;

    std::string cmd;
    if(*beg == '%')             // New label
    {
        beg++;
        cmd = GetWord(&beg);
        int label_num = FindLabel(cmd, labels, *n_labels);
        if(label_num < 0)
        {
            labels[*n_labels].name_      = cmd;
            labels[*n_labels].address_   = cur_cmd;
            (*n_labels)++;
        }
        return V_ERR::V_OK;
    }


    cmd = GetWord(&beg);
    /* */if(cmd == MOV_CMD)
    {
        int status = Mov(cmds, cur_cmd, beg, end);
        if(status < 0)
        return -V_ERR::V_INVALID_ARG;
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
        cmd = GetWord(&beg);
        int dst_reg_num = GetRegNum(cmd);
        if(dst_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after add\n";
            return -V_ERR::V_INVALID_ARG;
        }

        SkipSpaces(beg);
        if(*beg != ',')
        {
            std::cout << "expected ',' in mov []\n";
            return -V_ERR::V_INVALID_ARG;
        }

        beg++;
        SkipSpaces(beg);

        cmd = GetWord(&beg);
        int src_reg_num = GetRegNum(cmd);
        if(src_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after add\n";
            return -V_ERR::V_INVALID_ARG;
        }

        cmds[cur_cmd++] = ADD;
        cmds[cur_cmd++] = dst_reg_num;
        cmds[cur_cmd++] = src_reg_num;
    }
    else if(cmd == SUB_CMD)
    {
        cmd = GetWord(&beg);
        int dst_reg_num = GetRegNum(cmd);
        if(dst_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after add\n";
            return -V_ERR::V_INVALID_ARG;
        }

        SkipSpaces(beg);
        if(*beg != ',')
        {
            std::cout << "expected ',' in mov []\n";
            return -V_ERR::V_INVALID_ARG;
        }

        beg++;
        SkipSpaces(beg);

        cmd = GetWord(&beg);
        int src_reg_num = GetRegNum(cmd);
        if(src_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after add\n";
            return -V_ERR::V_INVALID_ARG;
        }

        cmds[cur_cmd++] = SUB;
        cmds[cur_cmd++] = dst_reg_num;
        cmds[cur_cmd++] = src_reg_num;
    }
    else if(cmd == DIV_CMD)
    {
        cmd = GetWord(&beg);
        int src_reg_num = GetRegNum(cmd);
        if(src_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after add\n";
            return -V_ERR::V_INVALID_ARG;
        }

        cmds[cur_cmd++] = DIV;
        cmds[cur_cmd++] = src_reg_num;
    }
    else if(cmd == MUL_CMD)
    {
        cmd = GetWord(&beg);
        int src_reg_num = GetRegNum(cmd);
        if(src_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after add\n";
            return -V_ERR::V_INVALID_ARG;
        }

        cmds[cur_cmd++] = MUL;
        cmds[cur_cmd++] = src_reg_num;
    }
    // =================================
    else if(cmd == JMP_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, JMP);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    else if(cmd == JE_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, JE);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    else if(cmd == JNE_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, JNE);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    else if(cmd == JA_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, JA);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    else if(cmd == JAE_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, JAE);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    else if(cmd == JB_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, JB);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    else if(cmd == JBE_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, JBE);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    // =================================
    else if(cmd == CALL_CMD)
    {
        int status = Jump(cmds, cur_cmd, beg, end, labels, *n_labels, CALL);
        if(status < 0)
        {
            std::cout << "Invalid jump at " << cur_cmd << " command\n";
            return -V_ERR::V_INVALID_ARG;
        }
    }
    else if(cmd == RET_CMD)
    {
        cmds[cur_cmd++] = RET;
    }
    else if(cmd == NOP_CMD)
    {
        cmds[cur_cmd++] = NOP;
    }

    while(*beg == ' ' && beg != end)      beg++;
    if(*beg == '\n' || *beg == '\0')
    {
        return V_ERR::V_OK;
    }
    else
    {
        std::cout << "Extra arguments found. Command number is " << cur_cmd << "\n";
        ASM_DBG std::cout << "beg = '" << beg << "' \n";
        return -V_ERR::V_INVALID_ARG;
    }
}

int FindLabel(std::string name, Label* labels, size_t n_labels)
{
    assert(labels);

    for(size_t i = 0; i < n_labels; i++)
        if(labels[i].name_ == name)
            return i;
    return -V_ERR::V_NOT_FOUND;
}

int Mov(int*& cmds, size_t& cur_cmd, char*& beg, char*& end)
{
    assert(cmds);
    assert(beg);
    assert(end);

    std::string cmd;
    // First - check if next symbol is a bracket

    SkipSpaces(beg);
    if(*beg == '[')
    {
        // Reciever - ram
        beg++;
        SkipSpaces(beg);
        cmd = GetWord(&beg);

        int dst_reg_num = GetRegNum(cmd);
        if(dst_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' in mov [ ]\n";
            return -V_ERR::V_INVALID_ARG;
        }

        SkipSpaces(beg);
        if(*beg != ']')
        {
            std::cout << "expected ']' in mov []\n";
            return -V_ERR::V_INVALID_ARG;
        }

        beg++;
        SkipSpaces(beg);
        if(*beg != ',')
        {
            std::cout << "expected ',' in mov []\n";
            return -V_ERR::V_INVALID_ARG;
        }

        beg++;
        SkipSpaces(beg);
        cmd = GetWord(&beg);

        int src_reg_num = GetRegNum(cmd);
        if(src_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' in mov []\n";
            return -V_ERR::V_INVALID_ARG;
        }

        cmds[cur_cmd++] = MOV_RAM_REG_REG;
        cmds[cur_cmd++] = dst_reg_num;
        cmds[cur_cmd++] = src_reg_num;

        return V_ERR::V_OK;
    }

    // Second - check if next symbol is a register

    cmd = GetWord(&beg);
    int dst_reg_num = GetRegNum(cmd);
    if(dst_reg_num < 0)
    {
        std::cout << "Invalid argument '" << cmd << "' after mov\n";
        return -V_ERR::V_INVALID_ARG;
    }

    SkipSpaces(beg);
    if(*beg != ',')
    {
        std::cout << "expected ',' in mov reg\n";
        return -V_ERR::V_INVALID_ARG;
    }

    beg++;
    SkipSpaces(beg);
    /* */if(*beg == '[')
    {
        // ram
        beg++;
        SkipSpaces(beg);

        cmd = GetWord(&beg);
        int src_reg_num = GetRegNum(cmd);
        if(src_reg_num < 0)
        {
            std::cout << "Invalid argument '" << cmd << "' after mov reg, [\n";
            return -V_ERR::V_INVALID_ARG;
        }

        SkipSpaces(beg);
        if(*beg != ']')
        {
            std::cout << "expected ']' in mov reg, []\n";
            return -V_ERR::V_INVALID_ARG;
        }

        beg++;
        cmds[cur_cmd++] = MOV_REG_RAM_REG;
        cmds[cur_cmd++] = dst_reg_num;
        cmds[cur_cmd++] = src_reg_num;

        return V_ERR::V_OK;
    }
    else if(isdigit(*beg))
    {
        cmd = GetWord(&beg);

        cmds[cur_cmd++] = MOV_REG_NUM;
        cmds[cur_cmd++] = dst_reg_num;
        cmds[cur_cmd++] = stoi(cmd);

        return V_ERR::V_OK;
    }

    cmd = GetWord(&beg);
    int src_reg_num = GetRegNum(cmd);
    if(src_reg_num < 0)
    {
        std::cout << "Invalid argument '" << cmd << "' after mov\n";
        return -V_ERR::V_INVALID_ARG;
    }

    cmds[cur_cmd++] = MOV_REG_REG;
    cmds[cur_cmd++] = dst_reg_num;
    cmds[cur_cmd++] = src_reg_num;

    return V_ERR::V_OK;
}

int Jump(   int*& cmds, size_t& cur_cmd, char*& beg, char*& end,
            Label* labels, size_t n_labels, int jmp)
{
    assert(cmds);
    assert(beg);
    assert(end);
    assert(labels);
    assert(n_labels);

    SkipSpaces(beg);
    if(*beg != '$')
    {
        std::cout << "Expected '$' before label\n";
        return -V_ERR::V_INVALID_ARG;
    }

    beg++;
    std::string cmd = GetWord(&beg);

    int label_num = FindLabel(cmd, labels, n_labels);
    ASM_DBG
    {
        if(label_num < 0)
            std::cout << "Label '" << cmd << "' not found\n";
    }

    cmds[cur_cmd++] = jmp;
    cmds[cur_cmd++] = (label_num >= 0) ? labels[label_num].address_ : -1;

    return V_ERR::V_OK;
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

    #undef Check

    return -V_ERR::V_NOT_FOUND;
}

std::string GetWord(char** beg)
{
    assert(beg);

    std::string word;
    SkipSpaces(*beg);
    while(!isspace(**beg))
    {
        word += **beg;
        (*beg)++;
    }

    return word;
}


