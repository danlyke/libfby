#include <tinyxml2.h>
#include <iostream>

using namespace std;
using namespace tinyxml2;

const char * GetNodeValue(const XMLDocument &doc, const char **path)
{
    const XMLNode *node = doc.FirstChildElement(*path);
    const XMLElement *element;
    path++;
    
    while (*path && NULL != (node = node->FirstChildElement(*path))
           && (element = node->ToElement()))
    {
        cout << "Searching under " << element->Name() << " for " << *path << endl;
        path++;
    }
    return node ? element->GetText() : "";
}

void DumpNode(XMLNode *node, int depth = 0)
{
    while (node)
    {
        for (int i = 0; i < depth; ++i)
        {
            cout << "  ";
        }
        XMLElement *element = node->ToElement();
        if (element)
        {
            cout << "Element: " << element->Name();
            for (const XMLAttribute *attribute = element->FirstAttribute(); attribute != NULL; attribute = attribute->Next())
            {
                cout << ", " << attribute->Name() << ":'" << attribute->Value() << "'";
            }
            cout << endl;
            DumpNode(element->FirstChild(), depth + 1);
        }
        XMLText *text = node->ToText();
        if (text)
        {
            cout << "Value: " << text->Value() << endl;
        }
        
        XMLComment *comment = node->ToComment();
        if (comment)
        {
            cout << "Comment: " << comment->Value() << endl;
        }
        
        node = node->NextSibling();
    }
}

int main(int argc, char **argv)
{
    const char *time_path[] = { "gpx", "metadata", "time", NULL };
    
    for (int arg = 1; arg < argc; ++arg)
    {
        XMLDocument doc;
        doc.LoadFile( argv[arg] );
        XMLElement *element = doc.RootElement();
        
        DumpNode(element);
        cout << "Time is " << GetNodeValue(doc, time_path) << endl;
    }
}
