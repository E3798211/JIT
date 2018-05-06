#include <iostream>

#include "VirtualProc/Proc/VirtualProc.h"
#include "VirtualProc/Asm/VirtualAsm.h"
// #include "Tree/Tree.h"
#include "RecDescend/RecDescend.h"

#include <cstring>
/*
 *
 *  You can insert data with "{ a | b | c }"
 *
 */

int Printer(const int& a, FILE* output)
{
    return fprintf(output, "{ 5 | 7 | 9 }");
}

/*
int Dump(const Token& a, FILE* output)
{
    return fprintf(output, "{ %s | %d | %d }", a.Name().c_str(), a.Type(), a.Value());
}
*/

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

    Tree<Token>* syntax_tree = BuildSyntaxTree();

    syntax_tree->CreateDotOutput(Dump);

    delete syntax_tree;
    // Tree<Token>::CreateDotOutput(syntax_tree, Dump);
}

