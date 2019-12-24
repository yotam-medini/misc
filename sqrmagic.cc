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
      int skip = (sz - 2)*(sz - 2)/2;
      for (int k = 1; k <= half; ++k) {
        pending.insert(pending.end(), bt.mid2 - skip - k);
        pending.insert(pending.end(), bt.mid2 + skip + k);
      }
      solved = backtrack(bt, pending);
    }
    return solved;
  }
  bool backtrack(const BT& bt, seti_t& pending) {
    bool ok = false;
    cout << "debug: #pending="<<pending.size() << "\n"; show();
    if (pending.empty()) {
      ok = true;
    } else {
      int x = bt.x0, y = bt.y0;
      for (; (x < bt.x0 + bt.sz) && (get(x, y) == -1); ++x) {}
      if (x == bt.x0 + bt.sz) {
        for (x = bt.x0; (y < bt.y0 + bt.sz) && (get(x, y) == -1); ++y) {}
      }
      for (seti_t::iterator pi = pending.begin(); (!ok) && (pi != pending.end()); ++pi) {
        int v = *pi;
        int vc = bt.sum2 - v;
        seti_t::iterator pic = pending.find(vc);
        int cx = x, cy = bt.y0 + bt.sz- 1;
        if (x == 0) {
          cx = bt.x0 + bt.sz - 1;
          if (y != bt.y0) {
            cy = y;
          }
        }
        set(x, y, v);
        set(cx, cy, vc);
        pending.erase(pi);
        pending.erase(pic);
        ok = backtrack(bt, pending);
        if (!ok) {
          set(x, y, -1);
          set(cx, cy, -1);
          pi = pending.insert(pending.end(), v);
          pending.insert(pending.end(), vc);
        }
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
