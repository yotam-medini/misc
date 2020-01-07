#include <iostream>
#include <map>
#include <string>
using namespace std;
typedef map<int, int> i2i_t;

static int _nparen(int n, i2i_t& memo) {
  int ret = 1;
  auto er = memo.equal_range(n);
  if (er.first == er.second) { // not found
    if (n > 1) {
      ret = 0;
      for (int m = 1; m <= n; ++m) {
        ret += _nparen(m - 1, memo) * _nparen(n - m, memo);
      }
    }
    memo.insert(er.first, i2i_t::value_type(n, ret));
  } else {
    ret = er.first->second;
  }
  return ret;
}

int nparen(int n) {
  i2i_t memo;
  return _nparen(n, memo);
}

int main(int argc, char **argv) {
  for (int ai = 1; ai < argc; ++ai) {
    int n = stoi(argv[ai]);
    cout << "nparen("<< n << ") = " << nparen(n) << '\n';
  }
  return 0;
}
