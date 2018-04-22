#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

enum Commands
{
    END = -1,

    MOV =  1,
    PUSH,       // reg
    POP,        // reg

    ADD,        // reg + reg
    SUB,        // reg - reg
    DIV,        // AX  / reg
    MUL,        // AX  * reg

    JMP,
    JE,
    JNE,
    JA,
    JAE,
    JB,
    JBE,

    CALL,
    RET
};

const std::string  MOV_CMD = "mov";
const std::string PUSH_CMD = "push";
const std::string  POP_CMD = "pop";

const std::string  ADD_CMD = "add";
const std::string  SUB_CMD = "sub";
const std::string  DIV_CMD = "div";
const std::string  MUL_CMD = "mul";

const std::string  JMP_CMD = "jmp";
const std::string   JE_CMD = "je";
const std::string  JNE_CMD = "jne";
const std::string   JA_CMD = "ja";
const std::string  JAE_CMD = "jae";
const std::string   JB_CMD = "jb";
const std::string  JBE_CMD = "jbe";

const std::string CALL_CMD = "call";
const std::string  RET_CMD = "ret";

enum REGS
{
    AX,
    BX,
    CX,
    DX,

    R8,
    R9,
    R10,
    R11,

    BP,
    SP,
    IP,

    N_REGS
};

const std::string  AX_REG   = "ax";
const std::string  BX_REG   = "bx";
const std::string  CX_REG   = "cx";
const std::string  DX_REG   = "dx";

const std::string  R8_REG   = "r8";
const std::string  R9_REG   = "r9";
const std::string R10_REG   = "r10";
const std::string R11_REG   = "r11";

const std::string  BP_REG   = "bp";
const std::string  SP_REG   = "sp";

#endif // COMMANDS_H_INCLUDED
