#include "IndexedRandomPool.hpp"

#include <cassert>
#include <random>

int main()
{
    IndexedRandomPool<int, const char *> pool;
    pool.reserve(3);

    assert(pool.empty());
    assert(pool.push(1, "one"));
    assert(pool.push(2, "two"));
    assert(pool.push(3, "three"));
    assert(!pool.push(2, "duplicate"));
    assert(pool.size() == 3);
    assert(pool.contains(2));
    assert(*pool.find(2) == "two");

    assert(pool.erase(2, 3));
    assert(!pool.contains(2));
    assert(pool.size() == 2);
    assert(!pool.erase(2, 3));

    std::mt19937 randomGenerator(42);
    const auto &randomValue = pool.random(randomGenerator);
    assert(randomValue != nullptr);

    pool.clear();
    assert(pool.empty());
}
