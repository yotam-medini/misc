// -*-c++-*-
// -*- c++ -*-
#if !defined(RATGEO_H)
#define RATGEO_H

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
    const ratq_t& xy(unsigned i) const { return _xy[i]; }
    void xy(unsigned i, ratq_t& v) { _xy[i] = v; }
  private:
    ratq_t _xy[2];
};

extern std::ostream& operator<<(std::ostream&, const RatPt2&);

extern RatPt2::ratq_t distance2(const RatPt2&, const RatPt2&);

// 2-dimensinal point
class RatLine2
{
  public:
    typedef RatPt2::ratq_t ratq_t;
    RatLine2(const RatPt2& pt0=RatPt2(0, 0), const RatPt2& pt1=RatPt2(0, 1)) :
       _pt{pt0, pt1}
       {}
    const RatPt2& pt(unsigned i) const { return _pt[i]; }
    void pt(unsigned i, RatPt2& p) { _pt[i] = p; }
    ratq_t distance2(const RatPt2& e) const;
    int side(const RatPt2& e) const; // 1, 0, -1
  private:
    ratq_t dist_numerator(const RatPt2& e) const;
    RatPt2 _pt[2];
};

extern std::ostream& operator<<(std::ostream&, const RatLine2&);

extern bool intersection(RatPt2&, const RatLine2& l0, const RatLine2& l1);

class RatSeg2: public RatLine2
{
 public:
    RatSeg2(const RatPt2& pt0=RatPt2(0, 0), const RatPt2& pt1=RatPt2(0, 1)) :
        RatLine2(pt0, pt1)
        {}
    RatPt2::ratq_t length2() const { return ::distance2(pt(0), pt(1)); }
};
extern std::ostream& operator<<(std::ostream&, const RatSeg2&);

extern bool intersection(RatPt2&, const RatLine2& line, const RatSeg2& seg);
extern bool intersection(RatPt2&, const RatSeg2& seg0, const RatSeg2& seg1);

#endif /* RATGEO_H */
