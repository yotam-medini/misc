// Convert NoFear HTMLs to LaTeX long tables
#include <cctype>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <unordered_set>

#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;

typedef unordered_set<string> sets_t;
sets_t tags_seen;

typedef vector<string> vs_t;
typedef unordered_set<string> sets_t;

class Property
{
 public:
    string key;
    vs_t values;
};
typedef vector<Property> vproperty_t;

class Classes
{
 public:
    Classes(const vs_t& _n=vs_t(), const sets_t& _c=sets_t()) :
        node(_n), context(_c) {}
    vs_t node;
    sets_t context;
};
typedef vector<Classes> vclasses_t;

void props_print(const vproperty_t& props)
{
    for (const Property& p: props)
    {
        cerr << "  " << p.key << ":";
        for (const string& v: p.values) { cerr << ' ' << v; }
        cerr << '\n';
    }
}

vs_t& string_split(vs_t& result, const string& s)
{
    result.clear();
    istringstream iss(s);
    copy(istream_iterator<string>(iss), istream_iterator<string>(),
        back_inserter(result));
    return result;
}

void props_get_classes(const vproperty_t& props, vs_t& classes)
{
    static const string s_class("class");
    classes.clear();
    for (const Property& prop: props)
    {
        if (prop.key == s_class)
        {
            for (const string &v: prop.values)
            {
                vs_t vclasses;
                string_split(vclasses, v);
                classes.insert(classes.end(), vclasses.begin(), vclasses.end());
            }
        }
    }
}

void sets_add(sets_t& sets, const vs_t &vs)
{
    for (const string& s: vs)
    {
        sets.insert(sets.end(), s);
    }
}

string xmlc2str(const xmlChar* x)
{
    string s((const char*)(x));
    return s;
}

string strip_old(const xmlChar* x)
{
    const char *p = (const char*)(x);
    char c;
    while (((c = *p) != '\0') && isspace(c))
    {
        ++p;
    }
    int sz = strlen(p);
    while ((sz > 0) && isspace(p[sz - 1]))
    {
        --sz;
    }
    string s(p, sz);
    return s;
}

string strip(const xmlChar* x)
{
    string s((const char*)(x));
    int sz = s.size();
    int ltrim = 0;
    while ((ltrim < sz) && (isspace(s[ltrim])))
    {
        ++ltrim;
    }
    while ((sz > ltrim) && isspace(s[sz - 1]))
    {
        --sz;
    }
    string sws = s.substr(ltrim, sz - ltrim);
    string ret;
    for (char c: sws)
    {
        if ((c == L'’') || ((unsigned char)(c) == 0xe2))
        {
            ret.push_back('\'');
        }
        else if ((0 < c) && (c < 0x7f))
        {
            ret.push_back(c);
        }
    }
    return ret;
}

class GState
{
 public:
    string act_x_scene_y;
};

class State
{
 public:
    State(GState& _gstate) :
        gstate(_gstate),
        in_table(false),
        in_div(false),
        line_number(false),
        td(-1)
    {
    }
    GState& gstate;
    bool in_table;
    bool in_div;
    bool line_number;
    int td;
    string td_text;
    vclasses_t classes;
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
    static const string s_class("class");
    static const string s_div("div");
    static const string s_i("i");
    static const string s_meta("meta");
    static const string s_p("p");
    static const string s_scan("scan");
    static const string s_stage("noFear__stage");
    static const string s_table("table");
    static const string s_td("td");
    static const string s_text("text");
    static const string s_tr("tr");
    static const string
        interior_header("interior-header__title__text__pagetitle");

    const string indent = string(2 * depth, ' ');
    const string tag((const char*)(p->name));
    if (false) { cerr << indent << tag << ", type=" << p->type << '\n'; }
    vproperty_t props;
    node_get_properties(props, p);
    vs_t classes;
    props_get_classes(props, classes);
    sets_t context = (depth == 0 ? sets_t() : state.classes.back().context);
    sets_add(context, classes);
    state.classes.push_back(Classes(classes, context));
    const bool is_stage = (tag == s_div) &&
       (context.find(s_stage) != context.end());
    const bool italize = (tag == s_i);
    const bool emphasize = state.in_table && (is_stage || italize);

    if (tag == s_meta)
    {
        ; 
        // cerr << "meta\n";
        // props_print(props);
    }
    if (tag == s_table)
    {
        state.in_table = true;
        // props_print(props);
        cout << "\\begin{longtable}{"
            "p{.50\\textwidth} p{0.04\\textwidth} p{.40\\textwidth} }\n";
    }
    if (tag == s_td)
    {
        state.td = -1;
        // props_print(props);
        state.td_text.clear();
        const Property& prop = props[0];
        if (prop.key == "class")
        {
            static const string orig("noFear__cell noFear__cell--original");
            static const string modern("noFear__cell noFear__cell--modern");
            const string v = prop.values[0];
            if (v == orig)
            {
                state.td = 0;
            }
            else if (v == modern)
            {
                state.td = 1;
            }
        }
    }
    if (emphasize)
    {
        state.td_text += "\\emph{";
    }
    if (tag == s_text)
    {
        if (state.td >= 0)
        {
            static const string s_number("noFear__number");
            static const string s_speaker("noFear__speaker");
            const string content = strip(p->content);
            if (context.find(s_number) != context.end())
            {
                //  '\\hspace*{-%s}\hbox to %s{\\texttt{%s}}'
                static const string width("20pt");
                static const string hsapce_b =
                    string("\\hspace*{-") + 
                    width + 
                    string("}\\hbox to ") +
                    width + 
                    string("{\\texttt{");
                static const string hspace_e("}}");
                state.td_text += hsapce_b + content + hspace_e;
            }
            else if (!content.empty())
            {
                if ((!state.td_text.empty()) && (!isspace(state.td_text.back())))
                {
                    state.td_text += ' ';
                }
                if (context.find(s_speaker) != context.end())
                {
                    state.td_text += 
                        string("\\textbf{") + content + string("}");
                }
                else
                {
                    state.td_text += content;
                }
            }
        }
    }
    if ((tag[0] == 'h') && (props.size() == 1))
    {
        const Property& prop = props[0];
        if ((prop.key == s_class) && (prop.values[0] == interior_header))
        {
            // cerr << "interior_header: " << prop.values[0] << '\n';
            string header = strip(p->children->content);
            if (state.gstate.act_x_scene_y != header)
            {
                state.gstate.act_x_scene_y = header;
                cout << "\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
                    "\\subsection{" << header << "}\n\n";
            }
        }
    }
    
    for (xmlNodePtr c = p->children; c; c = c->next)
    {
        node_traverse(state, c, depth + 1);
    }

    static const string newline_nl("\\newline\n");
    static const string newline_nl_rb = newline_nl + string("}");
    static const size_t newline_nl_sz = newline_nl.size();

    if ((tag == s_div) || (tag == s_p))
    {
        if (state.in_table)
        {
            state.td_text += newline_nl;
        }
    }
    if (emphasize)
    {
        state.td_text += "}";
    }
    if ((tag == s_td) && (state.td >= 0))
    {
        if (state.td > 0) { cout << "& &\n"; }
        size_t sz = state.td_text.size();
        if ((newline_nl_sz < sz) && 
            (state.td_text.substr(sz - newline_nl_sz) == newline_nl))
        {
            state.td_text.erase(sz - newline_nl_sz);
        }
        else if ((newline_nl_sz + 1 < sz) &&
            (state.td_text.substr(sz - (newline_nl_sz + 1)) == newline_nl_rb))
        {
            state.td_text.erase(sz - (newline_nl_sz + 1));
            state.td_text.push_back('}');
        }
        cout << state.td_text;
        
        state.td = -1;
    }
    if (tag == s_tr)
    {
        cout << "\n\\\\[6pt]\n";
    }
    if (tag == s_table)
    {
        cout << "\n\\end{longtable}\n";
        state.in_table = false;
    }
    state.classes.pop_back();
}

void traverse(GState& gstate, const htmlDocPtr p)
{
    State state(gstate);
    for (xmlNodePtr c = p->children; c; c = c->next)
    {
        node_traverse(state, c, 0);
    }
}


int nofear_to_latex(GState &gstate, const char *fn)
{
    int rc = 0;    
    int options = 
        XML_PARSE_RECOVER | 
        XML_PARSE_NOERROR |
        XML_PARSE_NOWARNING;
    xmlDoc *p = xmlReadFile(fn, nullptr, options);
    if (p)
    {
        traverse(gstate, p);
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
    GState gstate;

    for (int ai = 1; (rc == 0) && (ai < argc); ++ai)
    {
        rc = nofear_to_latex(gstate, argv[ai]);
    }

    return rc;
}
