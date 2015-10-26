#ifndef TREEBUILDER_H_INCLUDED
#define TREEBUILDER_H_INCLUDED
#include <algorithm>
#include <functional>
#include "fbyparsetree.h"

namespace Fby
{

    FBYCLASS(HTMLOutputter);

    class ParseTreeNode;


FBYCLASS(TreeBuilder) : BaseObj {
    protected:
        ParseTreeNodePtr rootNode;
        NodeStack_t nodeStack;
        ParseTreeNodePtr lastNode;
    public:
        TreeBuilder(); 
        bool HasA(const char *name);
        bool ReferencesImage();


        void ForEach(std::function<void (ParseTreeNodePtr)> f);
        void AsHTML(HTMLOutputter &outputter);
        ParseTreeNodePtr CurrentNode();
        bool Reuse(const char *name);
        void Push(ParseTreeNodePtr node);
        void Pop();
        void Pop(const string &name);
        void Graft(ParseTreeNodePtr node, int where);
        ParseTreeNodePtr RootNode();

        virtual void AddComment(const char * /* buffer */, size_t /* len */);
        virtual ParseTreeNode *NodeFactory(const std::string & /* nodename */);
    };

    class HTMLOutputterString : public HTMLOutputter {
    protected:
        std::ostream &os;
    public:
        HTMLOutputterString(ostream &os);
        virtual void AddString(const string &s);
        virtual void AddHTMLNodeBegin(const string &name, const vector< pair<string,string> > & attributes, bool empty_node);
        virtual void AddHTMLNodeEnd(const string &name);
        virtual void AddWikiLink(const string &wikiname, const string &text);
        virtual ~HTMLOutputterString();
    };

} // end of namespace Fby

#endif /* #ifndef TREEBUILDER_H_INCLUDED */
