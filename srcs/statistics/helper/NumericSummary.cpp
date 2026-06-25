#include "NumericSummary.hpp"

#include <algorithm>

void NumericSummary::add(double value)
{
    ++count;
    total += value;
    minimum = std::min(minimum, value);
    maximum = std::max(maximum, value);
}

void NumericSummary::clear()
{
    count = 0;
    total = 0.0;
    minimum = std::numeric_limits<double>::infinity();
    maximum = -std::numeric_limits<double>::infinity();
}

std::size_t NumericSummary::getCount() const
{
    return count;
}

double NumericSummary::getTotal() const
{
    return total;
}

double NumericSummary::getMinimum() const
{
    return isEmpty() ? 0.0 : minimum;
}

double NumericSummary::getMaximum() const
{
    return isEmpty() ? 0.0 : maximum;
}

double NumericSummary::getAverage() const
{
    return isEmpty() ? 0.0 : total / static_cast<double>(count);
}

bool NumericSummary::isEmpty() const
{
    return count == 0;
}
