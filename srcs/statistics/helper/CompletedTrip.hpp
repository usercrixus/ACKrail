#pragma once

#include "../../garage/engine/EnginePad.hpp"

class CompletedTrip
{
public:
    int engineId;
    int destinationStationId;
    EnginePad::TravelType travelType;
    double distanceKilometers;
    double travelTimeSeconds;
    double waitSeconds;
};
