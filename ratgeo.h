// -*-c++-*-
#include <gmpxx.h>

class RatPt2
{
  public:
     typedef mpq_class ratq_t;
     RatPt2(ratq_t x=0, ratq_t y=0) :
         _xy{x, y}
         {}
     ratq_t xy(unsigned i) const { return _xy[i]; }
     void xy(unsigned i, ratq_t& v) { _xy[i] = v; }
  private:
     ratq_t _xy[2];
};




