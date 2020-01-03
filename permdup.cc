#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;
typedef vector<string> vstr_t;
typedef map<char, int> c2i_t;

static void _permdup(vstr_t& res, const string& pfx, c2i_t& cc, unsigned target) {
  if (pfx.size() == target) {
    res.push_back(pfx);
  } else {
    string spfx(pfx);
    for (c2i_t::iterator i = cc.begin(), e = cc.end(); i != e; ++i) {
      int count = i->second;
      if (count > 0) {
        char c = i->first;
        spfx.push_back(c);
        --(i->second);
        _permdup(res, spfx, cc, target);
        ++(i->second);
        spfx.pop_back();
      }
    }
  }
}

void permdup(vstr_t& res, const string &s) {
  res.clear();
  c2i_t cc;
  for (char c: s) {
    auto er = cc.equal_range(c);
    if (er.first == er.second) {
      cc.insert(er.first, c2i_t::value_type(c, 1));
    } else {
      ++er.first->second;
    }
  }
  _permdup(res, string(""), cc, s.size());
}

int main(int argc, char **argv) {
  string s(argv[1]);
  vstr_t perms;
  permdup(perms, s);
  for (const string& perm: perms) {
    cout << perm << '\n';
  }
  return 0;
}
