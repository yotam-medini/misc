class Grid {
 public:
  virtual int nr() const  = 0;
  virtual int nc() const = 0;
  virtual int get(int i, int j) const = 0;
  virtual set(int i, int j, int val) = 0;
  bool valid(int i, int j) {
    return
      (0 <= i) && (i < nr()) &&
      (0 <= j) && (j < nc());
  }
};

void color_bdy(Grid& g) {
  for (int i = 0; i < g.nr(); ++i) {
    for (int j = 0; j < g.nc(); ++j) {
      if (g.get(i, j) == 1) {
        bool bdy = false;
        for (int si = -1; (si <= 1) && !bdy; ++si) {
          for (int sj = -1; (sj <= 1) && !bdy; ++sj) {
            bdy = (!g.valid()) || (g.get(i, j) == 0);
          }   
        }
        if (bdy) {
          g.set(i, j, 2);
        }
      }
    }
  }
}

void zero_interior(Grid& g) {
  for (int i = 0; i < g.nr(); ++i) {
    for (int j = 0; j < g.nc(); ++j) {
      if (g.get(i, j) == 1) {
        g.set(i, j, 0);
      }
    }
  }  
}

void recolor_bdy(Grid& g) {
  for (int i = 0; i < g.nr(); ++i) {
    for (int j = 0; j < g.nc(); ++j) {
      if (g.get(i, j) == 2) {
        g.set(i, j, 1);
      }
    }
  }  
}

void set_islands_boundaries(Grid& g) {
  color_bdy(g);
  zero_interior();
  recolor_bdy(g);
}
