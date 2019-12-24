#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
using namespace std;
typedef vector<int> vi_t;

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
   int mid = n/2;
   int n2 = n*n;
   int mid2 = n2/2;
   if (sz == 1) {
     set(mid, mid, mid2);
     solved = true;
   } else {
     int sum2 = 2*mid2;
     int sumsz = sz*mid2;
     int x0 = mid - sz/2, y0 = x0;
   }
   return solved;
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
