#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <fstream>
#include <map>

#include "fbyregexparser.h"


using namespace std;
using namespace Fby;

const char test_contents[] =
    "This is a <em>line</em> of text\n"
    "\n"
    "<blockquote>This is an indented line of text</blockquote>\n"
    "\n"
    "This is an unindented line of text\n"
    "\n";

class TestTreeBuilder : public TreeBuilder
{
public:
    TestTreeBuilder() : TreeBuilder() {};
    virtual ~TestTreeBuilder() {}
    ParseTreeNode *NodeFactory(const std::string & nodename)
    {
        return new ElementNode(nodename);
    }
};


int main(int argc, char** argv)
{
    setlocale(LC_ALL, ""); /* Use system locale instead of default "C" */

    TestTreeBuilder tree_builder;
    {
        MarkedUpTextParser parser;
        parser.Parse(tree_builder, test_contents, sizeof(test_contents) - 1);
    }
    cout << "Parsed" << endl;
    tree_builder.ForEach([](ParseTreeNodePtr n, int depth)
                         {
                             for (int i = 0; i < depth; ++i)
                                 cout << "  ";
                             cout << n->Name() << endl;
                         });

    
    
}
