#include "ratgeo.h"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& o, const RatPt2& p)
{
    return o << "(" << p.xy(0) << ", " << p.xy(1) << ")";
}

RatPt2::ratq_t distance2(const RatPt2& p0, const RatPt2& p1)
{
     RatPt2::ratq_t  dx = p1.xy(0) - p0.xy(0);
     RatPt2::ratq_t  dy = p1.xy(1) - p0.xy(1);
     RatPt2::ratq_t  d2 = dx*dx + dy*dy;
     return d2;
}

ostream& operator<<(ostream& o, const RatLine2& l)
{
    return o << "[L " << l.pt(0) << ", " << l.pt(1) << "]";
}

RatLine2::ratq_t RatLine2::distance2(const RatPt2& e) const
{
    const ratq_t dnum = dist_numerator(e);
    const ratq_t dnom2 = ::distance2(_pt[0], _pt[1]);
    const ratq_t ret = (dnum * dnum) / dnom2;
    return ret;
}

int RatLine2::side(const RatPt2& e) const
{
    const ratq_t dnum = dist_numerator(e);
    int ret = (dnum < 0 ? -1 : (dnum == 0 ? 0 : 1));
    return ret;
}

RatLine2::ratq_t RatLine2::dist_numerator(const RatPt2& e) const
{
    const ratq_t
        &x1 = _pt[0].xy(0), y1 = _pt[0].xy(1),
        &x2 = _pt[1].xy(0), y2 = _pt[1].xy(1);
    const ratq_t dnum =
        (y2 - y1)*e.xy(0) - (x2 - x1)*e.xy(1) + x2*y1 - y2*x1;
    return dnum;
}



bool intersection(RatPt2& result, const RatLine2& l0, const RatLine2& l1)
{
    typedef RatPt2::ratq_t ratq_t;

    // l0 -> (x1,y1)--(x2,y2)  & l1 -> (x2,y2)--(x3,y4)
    const ratq_t
        &x1 = l0.pt(0).xy(0), y1 = l0.pt(0).xy(1),
        &x2 = l0.pt(1).xy(0), y2 = l0.pt(1).xy(1),
        &x3 = l1.pt(0).xy(0), y3 = l1.pt(0).xy(1),
        &x4 = l1.pt(1).xy(0), y4 = l1.pt(1).xy(1);
    const ratq_t
        x1_x2 = x1 - x2,
        x3_x4 = x3 - x4,
        y1_y2 = y1 - y2,
        y3_y4 = y3 - y4;
    const ratq_t denom = x1_x2 * y3_y4 - y1_y2 * x3_x4;

    bool ret = (denom != 0);
    if (ret)
    {
        const ratq_t
            x1y2 = x1 * y2,
            y1x2 = y1 * x2,
            x3y4 = x3 * y4,
            y3x4 = y3 * x4;
        const ratq_t
            x1y2_y1x2 = x1y2 - y1x2,
            x3y4_y3x4 = x3y4 - y3x4;

        const ratq_t div_denom = 1 / denom;
        ratq_t resx = (x1y2_y1x2 * x3_x4 - x1_x2 * x3y4_y3x4) * div_denom;
        ratq_t resy = (x1y2_y1x2 * y3_y4 - y1_y2 * x3y4_y3x4) * div_denom;
        result = RatPt2(resx, resy);
    }
    return ret;
}

ostream& operator<<(ostream& o, const RatSeg2& s)
{
    return o << "[S " << s.pt(0) << ", " << s.pt(1) << "]";
}

static bool ptline_in_seg(const RatPt2& p, const RatLine2& line)
{
    bool inside = true;
    for (unsigned di = 0; (di < 2) && inside; ++di)
    {
        const RatPt2::ratq_t 
            &l0 = line.pt(0).xy(di),
            &l1 = line.pt(1).xy(di),
            &pq = p.xy(di);
        inside = 
            ((l0 <= pq) && (pq <= l1)) ||
            ((l1 <= pq) && (pq <= l0));
    }
    return inside;
}

bool intersection(RatPt2& p, const RatLine2& line, const RatSeg2& seg)
{
    const RatLine2& lseg = seg;
    bool ret = intersection(p, line, lseg) && ptline_in_seg(p, seg);
    return ret;
}

bool intersection(RatPt2& p, const RatSeg2& seg0, const RatSeg2& seg1)
{
    const RatLine2 &lseg0 = seg0, &lseg1 = seg1;
    bool ret = intersection(p, lseg0, lseg1) && 
        ptline_in_seg(p, seg0) &&
        ptline_in_seg(p, seg1);
    return ret;
}

#if defined(TEST)

#include <cstdlib>
#include <cstring>

static void usage(const char *p0)
{
    cerr <<
        "Usage:\n"
        "  " << p0 << "\n"
        "     [ppdist2 x1 y1 x2 y2]\n"
        "     [ppdist2 x1 x1d  y1 y1d  x2 x2d y2 y2d]\n"
        "     [lpdist2 x1 y1 x2 y2 x0 y0]\n"
        "     [lpdist2 x1 x1d  y1 y1d  x2 x2d  y2 y2d  x0 x0d  y0 y0d]\n"
        "     [intersection x1 y1 x2 y2 x3 y3 x4 y4]\n"
        "     [intersection "
        "  x1 x1d  y1 y1d  x2 x2d  y2 y2d  x3 x3d  y3 y3d  x4 x4d  y4 y4d]\n";
}

static RatPt2 args2pt(int argc /* 2 or 4 */, char **argv)
{
    bool denoms = (argc == 4);
    RatPt2::ratq_t  rq[2];
    for (unsigned rqi = 0, ai = 0; rqi < 2; ++rqi)
    {
        long numerator = strtol(argv[ai++], 0, 0);
        long denominator = 1;
        if (denoms)
        {
             denominator = strtol(argv[ai++], 0, 0);
        }
        rq[rqi] = RatPt2::ratq_t(numerator, denominator);
    }
    return RatPt2(rq[0], rq[1]);
}

static int test_pp_distance2(int argc, char **argv)
{
    int ret = (argc == 4 || argc == 8 ? 0 : 1);
    if (ret == 0)
    {
        const bool denoms = (argc == 8);
        const unsigned na = denoms ? 4 : 2;
        RatPt2 points[2];
        for (unsigned pi = 0, ai = 0; pi < 2; ++pi, ai += na)
        {
            points[pi] = args2pt(na, argv + ai);
        }
        RatPt2::ratq_t d2 = distance2(points[0], points[1]);
        cout << "distance2(" << points[0] << ", " << points[1] <<
            ") = " << d2 << "\n";
    }
    return ret;
}

static int test_lp_distance2(int argc, char **argv)
{
    int ret = (argc == 6 || argc == 12 ? 0 : 1);
    if (ret == 0)
    {
        const bool denoms = (argc == 12);
        const unsigned na = denoms ? 4 : 2;
        RatPt2 points[3];
        for (unsigned pi = 0, ai = 0; pi < 3; ++pi, ai += na)
        {
            points[pi] = args2pt(na, argv + ai);
        }
        const RatLine2 line(points[0], points[1]);
        RatPt2::ratq_t d2 = line. distance2(points[2]);
        cout << "distance2(" << line << ", " << points[2] <<
            ") = " << d2 << "\n";
    }
    return ret;
}

static int test_lp_side(int argc, char **argv)
{
    int ret = (argc == 6 || argc == 12 ? 0 : 1);
    if (ret == 0)
    {
        const bool denoms = (argc == 12);
        const unsigned na = denoms ? 4 : 2;
        RatPt2 points[3];
        for (unsigned pi = 0, ai = 0; pi < 3; ++pi, ai += na)
        {
            points[pi] = args2pt(na, argv + ai);
        }
        const RatLine2 line(points[0], points[1]);
        int side = line.side(points[2]);
        cout << "LinePtSide(" << line << ", " << points[2] <<
            ") = " << side << "\n";
    }
    return ret;
}

static int test_intersection(int argc, char **argv)
{
    int ret = ((argc == 8) or (argc == 16) ? 0 : 1);
    if (ret == 0)
    {
        const bool denoms = (argc == 16);
        const unsigned na = denoms ? 4 : 2;
        RatLine2 lines[2];
        int ai = 0;
        for (unsigned li = 0; li < 2; ++li)
        {
            RatPt2 points[2];
            for (unsigned pi = 0; pi < 2; ++pi)
            {
                points[pi] = args2pt(na, argv + ai);
                ai += na;
            }
            lines[li] = RatLine2(points[0], points[1]);
        }
        RatPt2 p;
        bool has = intersection(p, lines[0], lines[1]);
        cout << lines[0] << " & " << lines[1];
        if (has)
        {
            cout << " intersected @ " << p << "\n";
        }
        else
        {
            cout << " parallel\n";
        }
    }
    return ret;
}

static int test_lsintersection(int argc, char **argv)
{
    int ret = ((argc == 8) or (argc == 16) ? 0 : 1);
    if (ret == 0)
    {
        const bool denoms = (argc == 16);
        const unsigned na = denoms ? 4 : 2;
        RatLine2 line;
        RatSeg2 seg;
        int ai = 0;
        for (unsigned li = 0; li < 2; ++li)
        {
            RatPt2 points[2];
            for (unsigned pi = 0; pi < 2; ++pi)
            {
                points[pi] = args2pt(na, argv + ai);
                ai += na;
            }
            if (li == 0) 
            {
                line = RatLine2(points[0], points[1]);
            }
            else
            {
                seg = RatSeg2(points[0], points[1]);
            }
        }
        RatPt2 p;
        bool has = intersection(p, line, seg);
        cout << line << " & " << seg;
        if (has)
        {
            cout << " intersected @ " << p << "\n";
        }
        else
        {
            cout << " not intersected\n";
        }
    }
    return ret;
}

static int test_ssintersection(int argc, char **argv)
{
    int ret = ((argc == 8) or (argc == 16) ? 0 : 1);
    if (ret == 0)
    {
        const bool denoms = (argc == 16);
        const unsigned na = denoms ? 4 : 2;
        RatSeg2 segs[2];
        int ai = 0;
        for (unsigned li = 0; li < 2; ++li)
        {
            RatPt2 points[2];
            for (unsigned pi = 0; pi < 2; ++pi)
            {
                points[pi] = args2pt(na, argv + ai);
                ai += na;
            }
            segs[li] = RatSeg2(points[0], points[1]);
        }
        RatPt2 p;
        bool has = intersection(p, segs[0], segs[1]);
        cout << segs[0] << " & " << segs[1];
        if (has)
        {
            cout << " intersected @ " << p << "\n";
        }
        else
        {
            cout << " not intersected\n";
        }
    }
    return ret;
}

int main(int argc, char **argv)
{
     int ret = 1;
     if (argc < 2)
     {
         usage(argv[0]);
     }
     else
     {
         const char *test = argv[1];
         if (!strcmp(test, "ppdist2"))
         {
             ret = test_pp_distance2(argc - 2, argv + 2);
         }
         else if (!strcmp(test, "lpdist2"))
         {
             ret = test_lp_distance2(argc - 2, argv + 2);
         }
         else if (!strcmp(test, "lpside"))
         {
             ret = test_lp_side(argc - 2, argv + 2);
         }
         else if (!strcmp(test, "intersection"))
         {
             ret = test_intersection(argc - 2, argv + 2);
         }
         else if (!strcmp(test, "lsintersection"))
         {
             ret = test_lsintersection(argc - 2, argv + 2);
         }
         else if (!strcmp(test, "ssintersection"))
         {
             ret = test_ssintersection(argc - 2, argv + 2);
         }
         else
         {
             usage(argv[0]);
         }
     }
     return ret;
}

#endif /* TEST */
