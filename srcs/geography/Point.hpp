#pragma once

/**
 * Represents a two-dimensional geographic projection point.
 */
class Point
{
public:
    /**
     * Creates a point from projected coordinates.
     *
     * @param x Horizontal coordinate.
     * @param y Vertical coordinate.
     */
    Point(double x, double y);

    /**
     * Returns the horizontal coordinate.
     *
     * @return Horizontal coordinate.
     */
    double getX() const;

    /**
     * Returns the vertical coordinate.
     *
     * @return Vertical coordinate.
     */
    double getY() const;

private:
    double x;
    double y;
};
