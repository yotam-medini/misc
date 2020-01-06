#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <map>
using namespace std;
typedef int pair<int, int> ii_t;
typedef map<ii_t, int> ii2i_t;

static int _min_pali(const string& s, ii2i_t& memo, int l, int r) {
  int ret = 0;
  ii_t key{l, r};
  auto er = memo.equal_range(key);
  if (er.first == er.second) { // not found
    int sz = r + 1 -l;
    if (sz > 1) {
      if (s[l] == s[r]) {
        ret = _min_pali(s, memo, l + 1, r - 1);
      } else {
        int lret = _min_pali(s, memo, l + 1, r - 1);
        int rret = _min_pali(s, memo, l, r + 1);
        ret = (lret < rret ? lret : rret) + 1;
      }
    }
    memo.insert(er.first, i2i_t::value_type(key, ret));
  } else {
    ret = er.first->second;
  }
  return ret;
}

int min_pali(const string& s) {
  ii2i_t memo;
  int ret = (s.size() > 1) _min_pali(s, memo, 0, s.size() - 1) : 0);
  return ret;
}

int main(int argc, char** argv) {
  for (int ai = 1; ai < argc; ++ai) {
    const string s(argv[ai]);
    cout << "Min-deleteiom for Palyindrom (" << s << ") = " << min_pali(s);
  }
  return 0;
}
