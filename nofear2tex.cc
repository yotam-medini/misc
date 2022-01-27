// Convert NoFear HTMLs to LaTeX long tables
#include <iostream>
#include <unordered_set>
#include <expat.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

typedef unordered_set<string> sets_t;
sets_t tags_seen;

void start(void *userData, const char *name, const char *args[])
{
    const string sname(name);
    if (tags_seen.find(sname) == tags_seen.end())
    {
        tags_seen.insert(tags_seen.end(), sname);
        cerr << "tag: " << sname << "\n";
    }
    if (sname == string("table"))
    {
        cerr << "table\n";
    }
}
 
void value(void *userData, const char *val, int len)
{
    string sval(val, len);
    if (false) { cerr << "sval="<<sval << '\n'; }
}

void end(void *userData, const char *name)
{
    const string sname(name);
    if (sname == string("table"))
    {
        cerr << "table\n";
    }
}

int nofear_to_latex(XML_Parser parser, const char *fn)
{
    struct stat statbuf;
    int rc = stat(fn, &statbuf);
    if (rc == 0)
    {
        size_t fsz = statbuf.st_size;
        char *buf = new char[fsz + 1];
        FILE* f = fopen(fn, "r");
        size_t nb = fread(buf, sizeof(char), fsz, f);
        fclose(f);
        if (nb != fsz)
        {
            cerr << nb << " bytes read of " << fsz << " sized " << fn << '\n';
        }
        XML_Status rc_parse = XML_Parse(parser, buf, nb, XML_TRUE);
        cerr << "rc_parse=" << rc_parse << "\n";
    }
    else
    {
       cerr << "Failed to stat(" << fn << '\n'; 
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;

    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, start, end);
    XML_SetCharacterDataHandler(parser, value);
   
    for (int ai = 1; (rc == 0) && (ai < argc); ++ai)
    {
        rc = nofear_to_latex(parser, argv[ai]);
    }

    XML_ParserFree(parser);
    return rc;
}
