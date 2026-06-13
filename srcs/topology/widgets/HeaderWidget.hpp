#pragma once

class Garage;
class QPainter;
class Topology;

class HeaderWidget
{
public:
    static const double Height;
    static void draw(QPainter &painter, const Topology &topology, const Garage &garage, double width);
};
