#include "Geography.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

const double Geography::EarthRadiusKilometers = 6371.0088;
const double Geography::MaximumMercatorLatitude = 85.05112878;

double Geography::distanceKilometers(
    double latitude1,
    double longitude1,
    double latitude2,
    double longitude2)
{
    const double DegreesToRadians = std::numbers::pi / 180.0;

    const double latitude1Radians = latitude1 * DegreesToRadians;
    const double latitude2Radians = latitude2 * DegreesToRadians;
    const double latitudeDifference = (latitude2 - latitude1) * DegreesToRadians;
    const double longitudeDifference = (longitude2 - longitude1) * DegreesToRadians;

    const double latitudeSin = std::sin(latitudeDifference / 2.0);
    const double longitudeSin = std::sin(longitudeDifference / 2.0);
    const double haversine =
        latitudeSin * latitudeSin
        + std::cos(latitude1Radians) * std::cos(latitude2Radians)
            * longitudeSin * longitudeSin;
    const double boundedHaversine = std::clamp(haversine, 0.0, 1.0);
    const double centralAngle =
        2.0 * std::atan2(
                  std::sqrt(boundedHaversine),
                  std::sqrt(1.0 - boundedHaversine));

    return EarthRadiusKilometers * centralAngle;
}

Geography::ProjectedPosition Geography::webMercator(
    double latitude,
    double longitude)
{
    const double DegreesToRadians = std::numbers::pi / 180.0;

    const double boundedLatitude = std::clamp(
        latitude,
        -MaximumMercatorLatitude,
        MaximumMercatorLatitude);
    const double latitudeRadians = boundedLatitude * DegreesToRadians;

    return {
        longitude * DegreesToRadians,
        std::log(std::tan(std::numbers::pi / 4.0 + latitudeRadians / 2.0))};
}
