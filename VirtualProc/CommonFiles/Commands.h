#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

enum Commands
{
    END = -1,

    MOV_REG_NUM,        // mov ax, 0
    MOV_REG_RAM_REG,    // mov ax, [ bx ]
    MOV_REG_REG,        // mov ax, bx
    MOV_RAM_REG_REG,    // mov [ bx ], ax

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
    CMP,

    CALL,
    RET,

    IN,
    OUT,

    NOP = 90
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
const std::string  CMP_CMD = "cmp";

const std::string CALL_CMD = "call";
const std::string  RET_CMD = "ret";

const std::string   IN_CMD = "in";
const std::string  OUT_CMD = "out";

const std::string  NOP_CMD = "nop";
const std::string  END_CMD = "end";

enum REGS
{
    AX = 0,
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

    CMP_FLAG,   // cmp ax, bx : ( < 0) when ax < bx, 0 when ax == bx, ( > 0) when ax > bx

    N_REGS
};

const int EQUAL = 0;    ///<-- Flag value

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
