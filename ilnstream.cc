#include "ilnstream.h"
#include <sstream>

ilnstream::ilnstream(istream &vi) : i(vi), _ln(0), piline(0)
{
    next_line();
}

ilnstream::~ilnstream()
{
    delete piline;
}

bool ilnstream::eof() const
{
    return i.eof() && (!piline || piline->eof());
}

void ilnstream::next_line()
{
    if ((!piline || piline->eof()) && !i.eof())
    {
        delete piline;
        getline(i, line);
        ++_ln;
        piline = new istringstream(line);
    }
}

template <class T>
void getv(ilnstream& i, istream *pi, void (ilnstream::*pf_nl)(), T &v)
{
    bool got = false;
    while (!(got || i.eof()))
    {
         *pi >> v;
         got = !pi->fail();
         if (!got)
         {
             (i.*pf_nl)();
         }
    }
}

void ilnstream::get(std::string& v)
{
#if 1
    getv(*this, piline, &ilnstream::next_line, v);
#else
    bool got = false;
    while (!(got || eof()))
    {
         *piline >> v;
         got = !piline->fail();
         if (!got)
         {
             next_line();
         }
    }
#endif
}

void ilnstream::get(int& v)
{
    getv(*this, piline, &ilnstream::next_line, v);
}

void ilnstream::get(int long& v)
{
    getv(*this, piline, &ilnstream::next_line, v);
}

void ilnstream::get(int long long& v)
{
    getv(*this, piline, &ilnstream::next_line, v);
}

void ilnstream::get(unsigned int& v)
{
    getv(*this, piline, &ilnstream::next_line, v);
}

void ilnstream::get(unsigned int long& v)
{
    getv(*this, piline, &ilnstream::next_line, v);
}

void ilnstream::get(unsigned int long long& v)
{
    getv(*this, piline, &ilnstream::next_line, v);
}

ilnstream& operator>>(ilnstream &iln, std::string &v)
{
    iln.get(v);
    return iln;
}

ilnstream& operator>>(ilnstream &iln, int &v)
{
    iln.get(v);
    return iln;
}

ilnstream& operator>>(ilnstream &iln, int long &v)
{
    iln.get(v);
    return iln;
}

ilnstream& operator>>(ilnstream &iln, int long long &v)
{
    iln.get(v);
    return iln;
}

ilnstream& operator>>(ilnstream &iln, unsigned &v)
{
    iln.get(v);
    return iln;
}

ilnstream& operator>>(ilnstream &iln, unsigned long &v)
{
    iln.get(v);
    return iln;
}

ilnstream& operator>>(ilnstream &iln, unsigned long long &v)
{
    iln.get(v);
    return iln;
}

#if defined(TEST)

#include <fstream>
#include <iostream>

int main(int argc, char **argv)
{
    int rc = 0;
    {
        ofstream o("/tmp/iln");
        o << "\n\nabc def\n xyz\n";
    }
    {
        ifstream i("/tmp/iln");
        ilnstream iln(i);
        string s;
        for (unsigned si = 0; si < 4; ++si)
        {    
            iln >> s;
            cout << "si="<<si << ", s="<<s << ", ln=" << iln.ln() << 
                ", eof="<< int(iln.eof()) << "\n";
        }
    }

    return rc;
}

#endif
