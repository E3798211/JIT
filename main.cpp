#include <iostream>
#include "VirtualProc/Proc/VirtualProc.h"
#include "VirtualProc/Asm/VirtualAsm.h"

int main()
{
    Assembler("test");
    VirtualProc a("AsmOut");
    a.Run();
}
