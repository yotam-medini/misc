#include <string>
using namespace std;

bool isdigit(char c) {
  return ('0' <= c) && (c <= '9');
}

void get_run(const string& s, int& i, int& run, char& c) {
  int nr = 0;
  for (; isdigit(s[i]); ++i) {
    nr = 10*nr + (c - '0');
  }
  c = s[i++];
}

void get_mrun(const string& s, int& i, int& run, char c) {
  get_run(s, i, run, c);
  if (i < s.size()) {
    bool samec = true;
    while (samec) {
      int inext = i, runnext;
      char cnext;
      get_run(s, inext, runnext, cnext);
      samec = (c == cnext);
      if (samec)) {
        i = inext;
        run += runnext;
      }
    }
  }
}

bool runstreq(const string& s0, const string& s1) {
  bool eq = true;
  unsigned i0 = 0, i1 = 0, sz0 = s0.size(), sx1 = s1.size();
  while (eq && (i0 < sz0) && (i1 < sz1)) {
    int nr0, nr1;
    char c0, c1;
    get_mrun(s0, i0, nr0, c0);
    get_mrun(s1, i1, nr1, c1);
    eq = (nr0 == nr1) && (c0 == c1);
  }
  eq = eq && (i0 == sz0) && (i1 == sz1);
  return eq;
}
