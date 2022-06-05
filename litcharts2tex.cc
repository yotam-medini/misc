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
    // sets.insert(sets.end(), vs.begin(), vs.end());
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
    GState() : fn(""), html(false) {}
    const char *fn;
    bool html;
    string act_x_scene_y;
};

class State
{
 public:
    State(GState& _gstate) :
        gstate(_gstate),
        in_table(false),
        original(false),
        translation(false),
        line_number(-1)
    {
    }
    void push_classes(const vs_t& more_classes)
    {
         sets_t context = classes.empty() ? sets_t() : classes.back().context;
         sets_add(context, more_classes);
         classes.push_back(Classes(more_classes, context));
    }
    void write_location(ostream& os)
    {
        os << gstate.fn << ':' << line_number;
    }
    void pop_classes() { classes.pop_back(); }
    GState& gstate;
    bool in_table;
    bool original;
    bool translation;
    int line_number;
    vclasses_t classes;
};

static bool debug = false;
static bool debug_line_number = false;

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

bool strings_eq(const string& s0, const string& s1) {
   bool ret = (s0 == s1);
   return ret;
}

int get_line_number(const xmlNodePtr p)
{
    int line_number = -1;
    static const string data_line_count("data-line-count");
    for (xmlAttrPtr a = p->properties; a && (line_number == -1); a = a->next)
    {
        const string key = xmlc2str(a->name);
if (debug_line_number) { cerr << __func__ << " key=" << key << ", dlc="<<data_line_count <<'\n'; }
        if (key == data_line_count)
        // if (strings_eq(key, data_line_count))
     {
            const xmlNodePtr child = a->children;
            const char *cs_line_number =
                child ? "-1" : (const char*)(child->content);
            line_number = strtol(cs_line_number, nullptr, 0);
        }
    }
    return line_number;
}
string strip(string& s)
{
    size_t sz = s.size();
    size_t b = 0, e = sz;
    while ((b < sz) && (s[b] == '\n'))
    {
        ++b;
    }
    while ((b < e) && (s[e - 1] == '\n'))
    {
        --e;
    }
    return s.substr(b, e - b);
}

static void node_traverse(State& state, const xmlNodePtr p, size_t depth);

void traverse_children(State& state, const xmlNodePtr p, size_t depth)
{
    for (xmlNodePtr c = p->children; c; c = c->next)
    {
        node_traverse(state, c, depth);
    }
}

static bool vs_has(const vs_t& vs, const string& s)
{
    bool has = (find(vs.begin(), vs.end(), s) != vs.end());
    return has;
}

static bool sets_has(const sets_t& ss, const string& s)
{
    bool has = (ss.find(s) != ss.end());
    return has;
}

static void node_traverse(State& state, const xmlNodePtr p, size_t depth)
{
    static const string s_acs_character_heading("acs-character-heading");
    static const string s_br("br");
    static const string s_class("class");
    static const string s_col("col");
    static const string s_comparison_row("comparison-row");
    static const string s_div("div");
    static const string s_i("i");
    static const string s_main_title("main-title");
    static const string s_meta("meta");
    static const string s_modern_translaion("modern-translation");
    static const string s_original("original-content");
    static const string s_original_play("original-play");
    static const string s_p("p");
    static const string s_span("span");
    static const string s_stage_directions("stage-directions");
    static const string s_text("text");
    //static const string s_title("title");
    static const string s_shakespeare_translation_line(
        "shakespeare-translation-line");
    static const string s_translation("translation-content");
    static const string
        interior_header("interior-header__title__text__pagetitle");

    const string indent = string(2 * depth, ' ');
    const string tag((const char*)(p->name));
    if (false) { cerr << indent << tag << ", type=" << p->type << '\n'; }
    vproperty_t props;
    node_get_properties(props, p);
    vs_t classes;
    props_get_classes(props, classes);
    string class0 = classes.empty() ? string("") : classes[0];
    state.push_classes(classes);
    const sets_t& context = state.classes.back().context;
    if (debug) {
        string disp_classes;
        for (const string& cls: context)
        {
            disp_classes += cls + string(",");
        }
        const char *cs_tag = tag.c_str();
        const char *c0 = classes.empty() ? "" : classes[0].c_str();
        string content = (p->content && state.in_table
            ? xmlc2str(p->content) : string(""));
        cerr << "depth=" << depth << 
            ", tag=" << cs_tag << ", class=" << c0 << 
            ", context = " << (state.in_table ? disp_classes : string("")) <<
            ", content=" << content << '\n'; 
        // cerr << '\n';
    }
    const bool col_original = 
        vs_has(classes, s_col) && vs_has(classes, s_original_play);
    const bool col_translate =
        vs_has(classes, s_col) && vs_has(classes, s_modern_translaion);
    const char *safe_content = ((char *)(p->content) ? : "");
    if (class0 == s_main_title)
    {
        string header = xmlc2str(p->children->content);
        header = strip(header);
        state.gstate.act_x_scene_y = header;
        cout << "\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
           "\\subsection{" << header << "}\n\n";
    }
    if ((tag == s_div) && (class0 == s_comparison_row))
    {
        state.in_table = true;
        cout << "%% " << state.gstate.fn << ':' << p->line << '\n';
        cout << "\\begin{longtable}{"
            "p{.50\\textwidth} p{0.04\\textwidth} p{.40\\textwidth} }\n";
        debug_line_number = p->line >= 687;
        traverse_children(state, p, depth + 1);
        cout << "\n\\end{longtable}\n\n";
        debug_line_number = false;
        state.in_table = false;
    }
    else if ((tag == s_div) && col_original)
    {
        if (debug) { cerr << "original column\n"; }
        state.original = true;
        traverse_children(state, p, depth + 1);
        state.original = false;
        cout << " & & \n";
    }
    else if ((tag == s_div) && col_translate)
    {
        if (debug) { cerr << "translate column\n"; }
        state.translation = true;
        traverse_children(state, p, depth + 1);
        state.translation = false;
        cout << " \\\\ \n";
    }
    else if ((tag == s_p) && state.in_table)
    {
        const bool stage_direction = sets_has(context, s_stage_directions);
        const bool character_heading = vs_has(classes, s_acs_character_heading);
        if (stage_direction)
        {
            cout << "\\emph{" << safe_content;
        }
        else if (character_heading)
        {
            cout << "\\textbf{" << safe_content;
        }
        else
        {
            cout << safe_content;
        }
        traverse_children(state, p, depth + 1);
        if (stage_direction || character_heading)
        {
            cout << "}";
        }
        cout << "\n\n";        
    }
    else if ((tag == s_text) && state.in_table)
    {
        if (*safe_content)
        {
            const int line_number = state.line_number;
            if (line_number % 5 == 9)
            {
                cout << "\\hspace*{-20pt}\\hbox to 20pt{\\texttt{" << 
                    line_number << "}";
            }
            cout << safe_content;
        }
    }
    else if ((tag == s_span) && state.in_table)
    {
        cout << safe_content;
        bool translation_line = vs_has(classes, s_shakespeare_translation_line);
        if (translation_line)
        {
            state.line_number = get_line_number(p);
        }
        traverse_children(state, p, depth + 1);
        if (translation_line)
        {
            state.line_number = -1;
            cout << "\\newline\n % ";  state.write_location(cout);
        }
    }
    else if ((tag == s_br) && state.in_table)
    {
        cout << "\n\n";
    }
    else
    {
        traverse_children(state, p, depth + 1);
    }
    state.pop_classes();
}

static void traverse(GState& gstate, const htmlDocPtr p)
{
    State state(gstate);
    for (xmlNodePtr c = p->children; c; c = c->next)
    {
        node_traverse(state, c, 0);
    }
}


static int lc_to_latex(GState &gstate, const char *fn)
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

static int html_lc_to_latex(GState &gstate, const char *fn)
{
    int rc = 0;    
    htmlDocPtr p = htmlParseFile(fn, "utf-8");
    if (p)
    {
        traverse(gstate, p);
        // htmlFreeParserCtxt(p);
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
    int ai = 1;
    while ((ai < argc) && (argv[ai][0] == '-'))
    {
         const string opt(argv[ai]);
         if (opt == string("-debug"))
         {
             debug = true;
             ++ai;
         }
         else if (opt == string("-html"))
         {
             gstate.html = true;
             ++ai;
         }
         else
         {
             cerr << "Bad option: " << opt << '\n';
             ++ai;
             rc = 1;
         }
    }
    
    for (; (rc == 0) && (ai < argc); ++ai)
    {
        const char *fn = argv[ai];
        gstate.fn = fn;
        rc = (gstate.html 
            ? html_lc_to_latex(gstate, fn)
            : lc_to_latex(gstate, fn));
    }

    return rc;
}
