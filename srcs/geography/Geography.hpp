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
    static constexpr double EarthRadiusKilometers = 6371.0088;
    static constexpr double MaximumMercatorLatitude = 85.05112878;
};
