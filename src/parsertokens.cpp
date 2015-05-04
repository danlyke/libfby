#include "parsertokens.h"
using namespace std;

map<int, std::string> parserTokenNames;

void InitParserTokenNames()
{
    parserTokenNames[1] = "NEW_PARAGRAPH";
    parserTokenNames[2] = "ORDERED_LIST_ITEM";
    parserTokenNames[3] = "UNORDERED_LIST_ITEM";
    parserTokenNames[4] = "PARAGRAPH_TEXT";
    parserTokenNames[5] = "NAMED_ENTITY";
    parserTokenNames[6] = "NUMERIC_ENTITY";
    parserTokenNames[7] = "HEX_NUMERIC_ENTITY";
    parserTokenNames[8] = "HEADING1";
    parserTokenNames[9] = "HEADING2";
    parserTokenNames[10] = "HEADING3";
    parserTokenNames[11] = "HEADING4";
    parserTokenNames[12] = "HEADING5";
    parserTokenNames[13] = "HEADING6";
    parserTokenNames[14] = "TAG_OPEN";
    parserTokenNames[15] = "TAG_CLOSE";
    parserTokenNames[16] = "TAG_ATTRIBUTE";
    parserTokenNames[17] = "TAG_ATTRIBUTE_VALUE";
    parserTokenNames[18] = "TAG_ATTRIBUTE_VALUE_CLOSED";
    parserTokenNames[19] = "TAG_ATTRIBUTE_NAMED_ENTITY";
    parserTokenNames[20] = "TAG_ATTRIBUTE_NUMERIC_ENTITY";
    parserTokenNames[21] = "TAG_ATTRIBUTE_HEX_NUMERIC_ENTITY";
    parserTokenNames[22] = "TAG_ATTRIBUTE_CLOSE";
    parserTokenNames[23] = "WIKI_OPEN";
    parserTokenNames[24] = "WIKI_CLOSE";
    parserTokenNames[25] = "UNMATCHED";
}
