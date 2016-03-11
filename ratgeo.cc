#include "ratgeo.h"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& o, const RatPt2& p)
{
    return o << "[" << p.xy(0) << ", " << p.xy(1) << "]";
}

ostream& operator<<(ostream& o, const RatLine2& l)
{
    return o << "{L " << l.pt(0) << ", " << l.pt(1) << "}";
}


