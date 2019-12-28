#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstring>
using namespace std;
typedef vector<int> vi_t;

void a_show(const vi_t&a) {
  const char *sep = "";
  for (int x: a) {
    cout << sep << x;
  }
  cout << '\n';
}

void longest_alternate_subseq(vi_t& res, const vi_t& a) {
}

void naive_longest_alternate_subseq(vi_t& res, const vi_t& a) {
}

static int comp_test(const vi_t& a) {
  int rc = 0;
  vi_t res, naive_res;
  naive_longest_alternate_subseq(naive_res, a);
  longest_alternate_subseq(res, a);
  if (res != naive_res) {
    rc = 1;
    a_show(a);
  }
  return rc;
}

static int rand_test(int nt, unsigned sz) {
  int rc = 0;
  for (int t = 0; (rc == 0) && (t < nt); ++t) {
    vi_t a;
    while (a.size() < sz) {
      a.push_back(rand() % (2*sz));
    }
    comp_test(a);
  }
  return rc;
}

int main(int argc, char **argv) {
  int rc = 1;
  const char *cmd = argv[1];
  if (strcmp(cmd, "rand") == 0) {
    rc = rand_test(stoi(argv[2]), stoi(argv[3]));
  } else {
    vi_t a;
    for (int ai = 1; ai < argc; ++ai) {
      a.push_back(stoi(argv[ai]));
    }
    rc = comp_test(a);
  }
  return rc;
}

