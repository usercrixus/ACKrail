#pragma once

#include "Point.hpp"

class Geography
{
public:
    static double distanceKilometers(double latitude1, double longitude1, double latitude2, double longitude2);
    static Point webMercator(double latitude, double longitude);

private:
    static const double EarthRadiusKilometers;
    static const double MaximumMercatorLatitude;
};
