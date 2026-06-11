#include "./EngineFactory.hpp"

Biplace EngineFactory::createBiplace()
{
    Biplace engine;
    engine.setModelName(QStringLiteral("Rail Module X1"));
    engine.setMaximumSpeedKilometersPerHour(250.0);
    return engine;
}
