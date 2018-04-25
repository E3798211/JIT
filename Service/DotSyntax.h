#ifndef DOTSYNTAX_H_INCLUDED
#define DOTSYNTAX_H_INCLUDED

/*
 *
 *  You can insert data with "{ a | b | c }"
 *
 */

namespace DOT{

    const char DEFAULT_DOT_FILENAME[] = "Junk/dotdata.dot";
    const char DEFAULT_IMG_FILENAME[] = "Junk/treegraph.ps";

    const char OPEN[] = "xdg-open ";

    const char DOT1[] = "dot -Tps ";
    const char DOT2[] = " -o ";


    const char       BEGIN_GRAPH[]  = "digraph G {\n";
    const char         END_GRAPH[]  = "}\n";

    const char  SET_RECORD_SHAPE[]  = "node[shape = record];\n";

    const char BEGIN_DECLARATION[]  = " [label = \"";
    const char   END_DECLARATION[]  = "\"];\n";
    const char      BEGIN_COLUMN[]  = "{ ";
    const char        END_COLUMN[]  = " }";
    const char        NEXT_FIELD[]  = " | ";
    const char            LABELS[]  = "{ Pointer | Data | Left | Right } ";


    const char   RIGHT_DIRECTION[]  = " [color = green];\n";
    const char    LEFT_DIRECTION[]  = " [color = blue];\n";
    const char   WRONG_DIRECTION[]  = " [color = red];\n";

    const char                TO[]  = " -> ";

};

#endif // DOTSYNTAX_H_INCLUDED
