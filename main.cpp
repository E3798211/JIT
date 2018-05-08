#include <iostream>

#include "VirtualProc/Proc/VirtualProc.h"
#include "VirtualProc/Asm/VirtualAsm.h"
#include "RecDescend/Compiler.h"
#include "JIT/JIT.h"

int main()
{
/*
    Compile         ("test");
    Assembler       (COMPILED_CODE_FILENAME);
    VirtualProc a   ("AsmOut");
    a.Run();
*/

    asm volatile("nop\nnop\nnop\n");

    JitCompile("AsmOut");

    asm volatile("nop\nnop\nnop\n");

}

