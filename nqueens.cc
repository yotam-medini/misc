#include <iostream>
#include <vector>
#include <string>

using namespace std;
typedef vector<int> vi_t;

void _nqueens(vi_t& q, unsigned n) {
  if (q.size() == n) {
    const char* sep = "";
    for (unsigned c = 0; c < n; ++c) {
      int r = q[c] + 1;
      cout << sep << "abcdefgh"[c] << r; sep = " ";
    }
    cout << '\n';
  }
  int cnext = q.size();
  for (int rnext = 0; rnext < int(n); ++rnext) {
    bool rok = true;
    for (int c = 0; rok && (c < cnext); ++c) {
      rok = (rnext != q[c]) && 
        (cnext - rnext != c - q[c]) && // diagonal ./.
        (cnext + rnext != c + q[c]);   // diahonal .\.
    }
    if (rok) {
      q.push_back(rnext);
      _nqueens(q, n);
      q.pop_back();
    }
  }
}

void nqueens(int n) {
  cout << n << " Queens:\n";
  vi_t q;
  _nqueens(q, n);
}

int main(int argc, char** argv) {
  int n = stoi(argv[1]);
  nqueens(n);
  return 0;
}
