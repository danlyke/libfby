#include "fbytreebuilder.h"
using namespace Fby;

TreeBuilder::TreeBuilder() : BaseObj(BASEOBJINIT(TreeBuilder)),
                             rootNode(new ElementNode("div")),
                             nodeStack(),
                             lastNode(rootNode)
{
    nodeStack.push_back(rootNode);
    rootNode->AddAttribute("class");
    rootNode->AddAttributeValue("content");
};

void TreeBuilder::ForEach(std::function<void (ParseTreeNodePtr)> f)
{
    rootNode->ForEach(f);
}

bool TreeBuilder::HasA(const char *name)
{
    bool found(false);
    ForEach([&found, &name](ParseTreeNodePtr n)
            {
                if (n->Name() == name) found = true;
            });
    return found;
}

bool TreeBuilder::ReferencesImage()
{
    bool found(false);

    ForEach([&found](ParseTreeNodePtr n)
            {
                if (n->ReferencesImage())
                {
                    found = true;
                }
            });
    return found;
}
void TreeBuilder::AsHTML(HTMLOutputter &outputter)
{
    rootNode->AsHTML(outputter);
}

ParseTreeNodePtr TreeBuilder::CurrentNode()
{
    assert(!nodeStack.empty());
#if 0
    cout << "Got past node stack test " << nodeStack.size() << endl;
    for (auto n = nodeStack.begin() ; n != nodeStack.end(); ++n)
    {
        cout << "   " << (*n)->Name() << endl;
    }
#endif
    return nodeStack.back();
}

bool TreeBuilder::Reuse(const char *name)
{
    if (nodeStack.back()->HasChildren())
    {
        ParseTreeNodePtr child(nodeStack.back()->GetLastChild());
        if (child->Name() == name)
        {
            nodeStack.push_back(child);
            return true;
        }
    }
    return false;
}

void TreeBuilder::Push(ParseTreeNodePtr node)
{
    nodeStack.back()->AddChild(node);
//    cout << "Pushing node " << node->Name() << endl;
    nodeStack.push_back(node);
}

void TreeBuilder::Graft(ParseTreeNodePtr node, int where)
{
    nodeStack.front()->Insert(node, where);
}

void TreeBuilder::Pop()
{
    if (nodeStack.size() > 1)
    {
        ParseTreeNodePtr node = nodeStack.back();
//        cout << "Popping node " << node->Name() << endl;
        nodeStack.pop_back();
    }
}
void TreeBuilder::Pop(const string &name)
{
    NodeStack_t::const_reverse_iterator n;
    
    for (n = nodeStack.rbegin();
         n != nodeStack.rend(); ++n)
    {
        if ((*n)->Name() == name)
        {
            break;
        }
    }
    
    if (n != nodeStack.rend())
    {
        while (nodeStack.back()->Name() != name)
        {
            assert(nodeStack.size() > 2);
            nodeStack.pop_back();
        }
        assert(nodeStack.size() > 1);
        nodeStack.pop_back();
    }
}


ParseTreeNodePtr TreeBuilder::RootNode()
{
    return rootNode;
}



void TreeBuilder::AddComment(const char * /* buffer */, size_t /* len */)
{}

ParseTreeNode *TreeBuilder::NodeFactory(const std::string & /* nodename */)
{
    return NULL;
};



HTMLOutputterString::HTMLOutputterString(ostream &os)
    : HTMLOutputter(),
      os(os)
{
}

void HTMLOutputterString::AddString(const string &s)
{
    os << s;
}

bool EndTagShouldBeSuppressed(const string &name)
{
  return (name == "img") || (name == "br");
}

void HTMLOutputterString::AddHTMLNodeBegin(const string &name,
                                           const vector< pair<string,string> > & attributes,
                                           bool /* empty_node */)
{
    os <<  "<" << name;
    if (!attributes.empty())
    {
        for (auto attr = attributes.begin();
             attr != attributes.end(); ++ attr)
        {
            os <<  " " << attr->first << "=\"" << attr->second << "\"";
        }
    }
    //   if (empty_node)
    // os <<  "/";
    os <<  ">";
}

void HTMLOutputterString::AddHTMLNodeEnd(const string &name)
{
    if (!EndTagShouldBeSuppressed(name))
        os <<  "</" << name << ">";
    if (name == "p") os << endl << endl;
    if (name == "li") os << endl;
    if (name == "ul") os << endl;
    if (name == "ol") os << endl;
    if (name == "blockquote") os << endl;
}
void HTMLOutputterString::AddWikiLink(const string &wikiname,
                                      const string &text)
{
    os <<  "<a href=\"./" << wikiname << "\">" << text << "</a>";
}

HTMLOutputterString::~HTMLOutputterString()
{
}

