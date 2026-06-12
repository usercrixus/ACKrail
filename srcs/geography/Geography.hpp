#pragma once

#include "Point.hpp"

/**
 * Provides geographic distance and projection calculations.
 */
class Geography
{
public:
    /**
     * Calculates the great-circle distance between two geographic positions.
     *
     * @param latitude1 Latitude of the first position in degrees.
     * @param longitude1 Longitude of the first position in degrees.
     * @param latitude2 Latitude of the second position in degrees.
     * @param longitude2 Longitude of the second position in degrees.
     * @return Distance between the positions in kilometers.
     */
    static double distanceKilometers(double latitude1, double longitude1, double latitude2, double longitude2);

    /**
     * Projects a geographic position using the Web Mercator projection.
     *
     * Latitudes outside the supported Web Mercator range are clamped.
     *
     * @param latitude Latitude in degrees.
     * @param longitude Longitude in degrees.
     * @return Projected point in radians.
     */
    static Point webMercator(double latitude, double longitude);

private:
    static const double EarthRadiusKilometers;
    static const double MaximumMercatorLatitude;
};
