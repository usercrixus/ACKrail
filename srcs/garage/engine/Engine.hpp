#pragma once

#include "../../algorithm/Route.hpp"
#include "EnginePad.hpp"
#include <QString>

/**
 * Represents a rail engine and manages its movement along an assigned route.
 *
 * An engine stores its model specifications, current speed, trajectory, and
 * travel progress. Derived classes define specific rail module models.
 */
class Engine
{
public:
    /**
     * Destroys the engine.
     */
    virtual ~Engine();

    /** @return Unique engine identifier within its garage. */
    int getId() const;

    /**
     * Returns the engine model name.
     *
     * @return Engine model name.
     */
    const QString &getModelName() const;

    /**
     * Returns the physical engine length.
     *
     * @return Engine length in meters.
     */
    double getLengthMeters() const;

    /**
     * Returns the minimum empty distance required behind another engine.
     *
     * @return Security distance in meters.
     */
    double getSecurityDistanceMeters() const;

    /**
     * Returns the minimum interval between same-direction link entries.
     *
     * @return Entry separation in seconds.
     */
    double getEntrySeparationSeconds() const;

    /** @return Mutable journey execution state. */
    EnginePad &getPad();

    /** @return Read-only journey execution state. */
    const EnginePad &getPad() const;

    /**
     * Sets the engine model name.
     *
     * @param modelName New model name.
     */
    void setModelName(QString modelName);

    /**
     * Sets the maximum engine speed.
     *
     * @param maximumSpeedKilometersPerHour Maximum speed in kilometers per hour.
     */
    void setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour);

protected:
    /**
     * Creates an idle engine.
     *
     * @param id Unique engine identifier.
     */
    explicit Engine(int id);

    /**
     * Sets the physical engine length.
     *
     * Negative values are clamped to zero.
     *
     * @param lengthMeters Engine length in meters.
     */
    void setLengthMeters(double lengthMeters);

    /**
     * Sets the minimum empty distance required behind another engine.
     *
     * Negative values are clamped to zero.
     *
     * @param securityDistanceMeters Security distance in meters.
     */
    void setSecurityDistanceMeters(double securityDistanceMeters);

private:
    void updateEntrySeparationSeconds();

    const int id;
    QString modelName;
    double lengthMeters = 0.0;
    double securityDistanceMeters = 3.0;
    double entrySeparationSeconds = 0.0;
    EnginePad pad;
};
