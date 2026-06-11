#include "Point.hpp"

Point::Point(double x, double y)
    : x(x),
      y(y)
{
}

double Point::getX() const
{
    return x;
}

double Point::getY() const
{
    return y;
}
