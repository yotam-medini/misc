// istream with line coun

#include <iosfwd>
#include <string>

using namespace std;

class ilnstream
{
 public:
    ilnstream(istream &vi);
    virtual ~ilnstream();
    unsigned ln() const { return _ln; }
    void get(string&);
    void get(int&);
    void get(int long&);
    void get(int long long&);
    void get(unsigned int&);
    void get(unsigned int long&);
    void get(unsigned int long long&);
    bool eof() const;
 private:
    ilnstream(); // not available
    ilnstream(const ilnstream&); // not available
    ilnstream& operator=(const ilnstream&); // not available
    void next_line();
    istream &i;
    unsigned _ln;
    string line;
    istream* piline;
};

extern ilnstream& operator>>(ilnstream &, string &);
extern ilnstream& operator>>(ilnstream &, int &);
extern ilnstream& operator>>(ilnstream &, int long &);
extern ilnstream& operator>>(ilnstream &, int long long &);
extern ilnstream& operator>>(ilnstream &, unsigned &);
extern ilnstream& operator>>(ilnstream &, unsigned long &);
extern ilnstream& operator>>(ilnstream &, unsigned long long &);
