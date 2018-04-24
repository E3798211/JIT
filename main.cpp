#include <iostream>

#include "VirtualProc/Proc/VirtualProc.h"
#include "VirtualProc/Asm/VirtualAsm.h"
#include "Tree/Tree.h"

int Printer(Tree<int>* node)
{
    std::cout << "Node = " << node << "\n";
    return 0;
}

int main()
{
/*
    Assembler("test");
    VirtualProc a("AsmOut");
    a.Run();
*/
    Tree<int>* b1 = new Tree<int>;

    b1->InfixVisitor(Printer);

    delete b1;
}

