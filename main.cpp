#include <iostream>

#include "VirtualProc/Proc/VirtualProc.h"
#include "VirtualProc/Asm/VirtualAsm.h"
// #include "RecDescend/RecDescend.h"
#include "RecDescend/Compiler.h"

#include <cstring>

int main()
{
/*
    Assembler("test");
    VirtualProc a("AsmOut");
    a.Run();
*/
/*
    Tree<int>* b1 = new Tree<int>;
    Tree<int>* b2 = new Tree<int>;
    Tree<int>* b3 = new Tree<int>;

    b1->Left(b2);
    b1->Right(b3);

    b1->CreateDotOutput(Printer);

    delete b1;
*/

    // LoadProgramm("test");

    // Tree<Token>* syntax_tree = BuildSyntaxTree();

    // syntax_tree->CreateDotOutput(Dump);

    // delete syntax_tree;
    // Tree<Token>::CreateDotOutput(syntax_tree, Dump);

    Compile         ("test");
    Assembler       (COMPILED_CODE_FILENAME);
    VirtualProc a   ("AsmOut");
    a.Run();

}

