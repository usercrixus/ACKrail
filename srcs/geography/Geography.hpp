#pragma once

class Geography
{
public:
    struct ProjectedPosition
    {
        double x;
        double y;
    };

    static double distanceKilometers(double latitude1, double longitude1, double latitude2, double longitude2);
    static ProjectedPosition webMercator(double latitude, double longitude);

private:
    static const double EarthRadiusKilometers;
    static const double MaximumMercatorLatitude;
};
