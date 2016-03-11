// -*-c++-*-
#include <iosfwd>
#include <gmpxx.h>

// 2-dimensinal point
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

// 2-dimensinal point
class RatLine2
{
  public:
     RatLine2(const RatPt2& pt0=RatPt2(0, 0), const RatPt2& pt1=RatPt2(0, 1)) :
        _pt{pt0, pt1}
        {}
     const RatPt2& pt(unsigned i) const { return _pt[i]; }
     void pt(unsigned i, RatPt2& p) { _pt[i] = p; }
  private:
     RatPt2 _pt[2];
};



