#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cstdlib>
using namespace std;
typedef vector<int> vi_t;
typedef set<int> seti_t;

class BT {
 public:
  BT(int n, int _sz) :
   sz(_sz), mid(n/2), n2(n*n), mid2(n2/2), sum2(2*mid), sumsz(sz*mid2),
   x0(mid - sz/2), y0(0)
   {}
   int sz, mid, n2, mid2, sum2, sumsz, x0, y0;
};

class SqrMagic {
 public:
  SqrMagic(int _n) : n(_n), _a(n*n, -1) {
    if (n % 2 == 1) { solve(); }
  }
  void show() const {
    for (int y = n - 1; y >= 0; --y) {
      const char *sep = "";
      for (int x = 0; x < n; ++x) {
        cout << sep << setw(2) << get(x, y); sep = " ";
      }
      cout << '\n';
    }
  }
 private:
  void solve() {
    bool ok = true;
    for (int sz = 1; ok && (sz <= n); sz += 2) {
      ok = solve_bdy(sz);
    }
  }
  bool solve_bdy(int sz) {
    bool solved = false;
    BT bt{n, sz};
    if (sz == 1) {
      set(bt.mid, bt.mid, bt.mid2);
      solved = true;
    } else {
      seti_t pending;
      int half = 2*(sz - 1);
      for (int k = 0; k < half; ++k) {
        pending.insert(pending.end(), k);
        pending.insert(pending.end(), bt.n2 - k - 1);
      }
      solved = backtrack(bt, pending);
    }
    return solved;
  }
  bool backtrack(const BT& bt, seti_t& pending) {
    bool ok = false;
    cout << "debug: #pending="<<pending.size(); << "\n"; show();
    if (pending.empty()) {
      ok = True;
    } else {
      int x = bt.x0, y = bt.y0;
      for (; (x < x0 + bt.sz) && (get(x, y) == -1); ++x) {}
      if (x == x0 + bt.sz) {
        for (x = x0; (y < y0 + bt.sz) && (get(x, y) == -1); ++y) {}
      }
      for (seti_t::iterator pi = pending.begin(); (!ok) && (pi != pending.end());) {
        if (pi != pending.end()) {
          ++pi;
        }
        int v = *pi;
        set(x, y, v);
        int vc = bt.sum2 - v;
        
      }
    }
    return ok;
  }
  int get(int x , int y) const { return _a[ixy(x, y)]; }
  void set(int x, int y, int v) { _a[ixy(x, y)] = v; }
  int ixy(int x, int y) const { return n*x + y; }
  int n;
  vi_t _a;
};

int main(int argc, char **argv) {
  int rc = 0;
  int n = stoi(argv[1]);
  SqrMagic sqm{n};
  sqm.show();
  return rc;
}
