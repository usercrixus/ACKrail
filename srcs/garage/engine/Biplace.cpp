#include "./Biplace.hpp"

Biplace::Biplace(int id)
    : Engine(id)
{
    setModelName(QStringLiteral("Rail Module X1"));
    setLengthMeters(3.0);
    setMaximumSpeedKilometersPerHour(250.0);
}
