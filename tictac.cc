#include <iostream>
#include <map>
using namespace std;
typedef unsigned u_t;

static const u_t TIC_EMPTY = 0;
static const u_t TIC_O = 1;
static const u_t TIC_X = 2;

typedef enum {TIC_TIE, TIC_O_WON, TIC_X_WON} tic_res_t;

class Grid {
 public:
  Grid(u_t _u=0) : u(_u) {}
  u_t get(u_t x, u_t y) const { return (u >> shift(x, y)) & 3; }
  void set(u_t x, u_t y, u_t v) { u |= ((v & 3) << shift(x, y)); }
  u_t getu() const { return u; }
  void print(ostream& os=cout) const {
    os << ".---.\n";
    for (u_t y = 3; y > 0;) {
      --y;
      os << '|';
      for (u_t x = 0; x < 3; ++x) {
        os << " OX"[get(x, y)];
      }
      os << "|\n";
    }
    os << ".---.\n";
  }
private:
 static u_t shift(u_t x, u_t y) { return 2*(3*x+y); } 
 u_t u;
};
typedef map<Grid, int> grid2u_t; // -1 or u(x,y) who wins ?
bool operator<(const Grid& g0, const Grid& g1) {
  return g0.getu() < g1.getu(); 
}
 
static void get_xy(u_t& x, u_t& y, int u) { x = u / 3; y = u % 3; }

bool ox_won(const Grid& g, u_t ox) {
  bool won = false;
  for (u_t x = 0; (x != 3) && !won; ++x) {
    won = true;
    for (u_t y = 0; (y != 3) && won; ++y) {
      won = (g.get(x, y) == ox);
    }
  }
  for (u_t y = 0; (y != 3) && !won; ++y) {
    won = true;
    for (u_t x = 0; (x != 3) && won; ++x) {
      won = (g.get(x, y) == ox);
    }
  }
  won = won || ((g.get(1, 1) == ox) && (
    ((g.get(0, 0) == ox) && (g.get(2, 2) == ox)) ||
    ((g.get(2, 0) == ox) && (g.get(0, 2) == ox))));
  return won;
}

bool o_won(const Grid& g) {
  return ox_won(g, TIC_O);
}
bool x_won(const Grid& g) {
  return ox_won(g, TIC_X);
}
bool is_tie(const Grid& g) {
  bool tie = true;
  for (u_t x = 0; tie && (x != 3); ++x) {
    for (u_t y = 0; tie && (y != 3); ++y) {
      tie = (g.get(x, y) != 0);
    }
  }
  tie = tie && (!o_won(g)) && (!x_won(g));
  return tie;
}

tic_res_t ox_best(const Grid& g, grid2u_t& memo, int ox, u_t depth=0) {
  tic_res_t ret = TIC_TIE;
  static u_t max_depth = 0, calls = 0;
  ++depth;
  if (max_depth < depth) {
    max_depth = depth;
    cerr << "depth="<<depth<<'\n';
  }
  ++calls;
  if ((calls & (calls - 1)) == 0) {
    cerr << "calls="<<calls<<'\n';
  }
  int opponent = (ox == TIC_O ? TIC_X : TIC_O);
  auto er = memo.equal_range(g);
  grid2u_t::iterator iter = er.first;
  bool won = false;
  int ixy = - 1;
  if (er.first == er.second) { // not yet in memo
    Grid gnext(g);
    for (int x = 0; (x != 3) && !won; ++x) {
      for (int y = 0; (y != 3) && !won; ++y) {
        if (g.get(x, y) == TIC_EMPTY) {
          gnext = g;
          gnext.set(x, y, ox);
          ret = ox_best(gnext, memo, opponent, depth);
          if (won) {
            ixy = 3*x + y;
          }
        }
      }
    }
    memo.insert(iter, grid2u_t::value_type(gnext, ixy));
  } else {
    won = (iter->second) >= 0;
  }
  return ret;
};

int main(int argc, char** argv) {
  int rc = 0;
  grid2u_t memo;
  u_t uini = (argc > 1 ? stoi(argv[1]) : 0);
  cerr << "uini="<<uini << '\n';
  Grid g(uini);
  g.print();

    while (!(o_won(g) || x_won(g) || is_tie(g))) {
      u_t x, y;
      grid2u_t::const_iterator iter = memo.find(g);
      int u = iter->second;
      get_xy(x, y, u);
      cerr << "u="<<u<< ", x="<<x << ", y="<<y << '\n';
      g.set(x, y, TIC_O);
      cerr << "g.u=" << g.getu() << '\n';
      g.print();
      cerr << "Enter x, y: "; cerr.flush();
      cin >> x >> y;
      cerr << "You entered: x="<<x << ", y="<<y<<'\n';
      g.set(x, y, TIC_X);
    }

  return rc;
}
