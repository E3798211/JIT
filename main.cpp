#include <iostream>

#include "VirtualProc/Proc/VirtualProc.h"
#include "VirtualProc/Asm/VirtualAsm.h"
// #include "Tree/Tree.h"
#include "RecDescend/RecDescend.h"


/*
 *
 *  You can insert data with "{ a | b | c }"
 *
 */

int Printer(const int& a, FILE* output)
{
    return fprintf(output, "{ 5 | 7 | 9 }");
}

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

    LoadProgramm("test");
    BuildSyntaxTree();

}

