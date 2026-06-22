#pragma once

#include <cstddef>
#include <limits>

class NumericSummary
{
public:
    void add(double value);
    void clear();

    std::size_t getCount() const;
    double getTotal() const;
    double getMinimum() const;
    double getMaximum() const;
    double getAverage() const;
    bool isEmpty() const;

private:
    std::size_t count = 0;
    double total = 0.0;
    double minimum = std::numeric_limits<double>::infinity();
    double maximum = -std::numeric_limits<double>::infinity();
};
