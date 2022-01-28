// Convert NoFear HTMLs to LaTeX long tables
#include <iostream>
#include <vector>
#include <unordered_set>
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;

typedef unordered_set<string> sets_t;
sets_t tags_seen;

typedef vector<string> vs_t;

class Property
{
 public:
    string key;
    vs_t values;
};
typedef vector<Property> vproperty_t;

void props_print(const vproperty_t& props)
{
    for (const Property& p: props)
    {
        cerr << "  " << p.key << ":";
        for (const string& v: p.values) { cerr << ' ' << v; }
        cerr << '\n';
    }
}

string xmlc2str(const xmlChar* x)
{
    string s((const char*)(x));
    return s;
}

class State
{
 public:
    State() :
        in_table(false),
        in_div(false),
        line_number(false),
        td(-1)
    {
    }
    string act_x_scene_y;
    bool in_table;
    bool in_div;
    bool line_number;
    int td;
};

void node_get_properties(vproperty_t& props, const xmlNodePtr p)
{
    props.clear();
    for (xmlAttrPtr a = p->properties; a; a = a->next)
    {
        Property property;
        property.key = xmlc2str(a->name);
        for (xmlNodePtr v = a->children; v; v = v->next)
        {
            property.values.push_back(xmlc2str(v->content));
        }
        props.push_back(property);
    }
}

void node_traverse(State& state, const xmlNodePtr p, size_t depth)
{
    static const string s_meta("meta");
    static const string s_table("table");
    static const string s_div("div");
    static const string s_scan("scan");
    static const string s_tr("tr");
    static const string s_td("td");

    const string indent = string(2 * depth, ' ');
    const string tag((const char*)(p->name));
    cerr << indent << tag << ", type=" << p->type << '\n';
    bool is_table = false, is_div = false;
    if (tag == s_meta)
    {
        cerr << "meta\n";
        vproperty_t props;
        node_get_properties(props, p);
        props_print(props);
    }
    for (xmlNodePtr c = p->children; c; c = c->next)
    {
        node_traverse(state, c, depth + 1);
    }
}

void traverse(const htmlDocPtr p)
{
    State state;
    for (xmlNodePtr c = p->children; c; c = c->next)
    {
        node_traverse(state, c, 0);
    }
}

#if 0
void element_traverse(const xmlDtdPtr p, size_t depth)
{
    cout << string(2 * depth, ' ') << p->name << '\n';
#if 0
    for (xmlDtdPtr c = p->children; c; c = c->next)
    {
        element_traverse(c, depth + 1);
    }
#endif
}

void etraverse(const htmlDocPtr p)
{
    for (xmlDtdPtr e = p->intSubset; e; e = e->next)
    {
        element_traverse(e, 0);
    }
}
#endif

int nofear_to_latex(const char *fn)
{
    int rc = 0;    
    // htmlDocPtr p = htmlParseFile(fn, nullptr);
#if 0
    int options = HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING;
    htmlDocPtr p = htmlReadFile(fn, nullptr, options);
#else
    int options = 
        XML_PARSE_RECOVER | 
        XML_PARSE_NOERROR |
        XML_PARSE_NOWARNING;
    xmlDoc *p = xmlReadFile(fn, nullptr, options);
#endif
    if (p)
    {
        traverse(p);
        xmlFreeDoc(p);
    }
    else
    {
        cerr << "Failed to parse " << fn << '\n';
        rc = 1;
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;

    for (int ai = 1; (rc == 0) && (ai < argc); ++ai)
    {
        rc = nofear_to_latex(argv[ai]);
    }

    return rc;
}
