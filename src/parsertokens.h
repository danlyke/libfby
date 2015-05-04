#include <map>
#include <string>

#define NEW_PARAGRAPH 1
#define ORDERED_LIST_ITEM 2
#define UNORDERED_LIST_ITEM 3
#define PARAGRAPH_TEXT 4
#define NAMED_ENTITY 5
#define NUMERIC_ENTITY 6
#define HEX_NUMERIC_ENTITY 7
#define HEADING1 8
#define HEADING2 9
#define HEADING3 10
#define HEADING4 11
#define HEADING5 12
#define HEADING6 13
#define TAG_OPEN 14
#define TAG_CLOSE 15
#define TAG_ATTRIBUTE 16
#define TAG_ATTRIBUTE_VALUE 17
#define TAG_ATTRIBUTE_VALUE_CLOSED 18
#define TAG_ATTRIBUTE_NAMED_ENTITY 19
#define TAG_ATTRIBUTE_NUMERIC_ENTITY 20
#define TAG_ATTRIBUTE_HEX_NUMERIC_ENTITY 21
#define TAG_ATTRIBUTE_CLOSE 22
#define WIKI_OPEN 23
#define WIKI_CLOSE 24
#define UNMATCHED 25


extern std::map<int, std::string> parserTokenNames;

extern void InitParserTokenNames();
