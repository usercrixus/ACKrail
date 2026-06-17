#pragma once

#include <cstddef>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * Stores values in a vector and keeps a key-to-index map for O(1) removal.
 *
 * This is useful when a collection needs fast random selection and fast removal
 * by identifier. Removal swaps the removed value with the last vector entry, so
 * iteration order is not stable.
 */
template <typename Key, typename Value>
class IndexedRandomPool
{
public:
    void reserve(std::size_t count)
    {
        values.reserve(count);
        indexByKey.reserve(count);
    }

    bool empty() const
    {
        return values.empty();
    }

    std::size_t size() const
    {
        return values.size();
    }

    void clear()
    {
        values.clear();
        indexByKey.clear();
    }

    bool contains(const Key &key) const
    {
        return indexByKey.find(key) != indexByKey.end();
    }

    const Value &at(const Key &key) const
    {
        return values.at(indexByKey.at(key));
    }

    Value &at(const Key &key)
    {
        return values.at(indexByKey.at(key));
    }

    const Value &operator[](std::size_t index) const
    {
        return values[index];
    }

    Value &operator[](std::size_t index)
    {
        return values[index];
    }

    const Value *find(const Key &key) const
    {
        const auto position = indexByKey.find(key);
        return position == indexByKey.end() ? nullptr : &values[position->second];
    }

    Value *find(const Key &key)
    {
        const auto position = indexByKey.find(key);
        return position == indexByKey.end() ? nullptr : &values[position->second];
    }

    bool push(const Key &key, const Value &value)
    {
        if (contains(key))
            return false;
        indexByKey.emplace(key, values.size());
        values.push_back(value);
        return true;
    }

    bool push(const Key &key, Value &&value)
    {
        if (contains(key))
            return false;
        indexByKey.emplace(key, values.size());
        values.push_back(std::move(value));
        return true;
    }

    bool erase(const Key &key, const Key &movedKey)
    {
        const auto position = indexByKey.find(key);
        if (position == indexByKey.end())
            return false;

        const std::size_t removedIndex = position->second;
        const std::size_t lastIndex = values.size() - 1;
        if (removedIndex != lastIndex)
        {
            values[removedIndex] = values[lastIndex];
            indexByKey[movedKey] = removedIndex;
        }
        values.pop_back();
        indexByKey.erase(position);
        return true;
    }

    const Value &back() const
    {
        return values.back();
    }

    Value &back()
    {
        return values.back();
    }

    template <typename RandomGenerator>
    const Value &random(RandomGenerator &randomGenerator) const
    {
        std::uniform_int_distribution<std::size_t> distribution(0, values.size() - 1);
        return values[distribution(randomGenerator)];
    }

    template <typename RandomGenerator>
    Value &random(RandomGenerator &randomGenerator)
    {
        std::uniform_int_distribution<std::size_t> distribution(0, values.size() - 1);
        return values[distribution(randomGenerator)];
    }

    auto begin()
    {
        return values.begin();
    }

    auto begin() const
    {
        return values.begin();
    }

    auto end()
    {
        return values.end();
    }

    auto end() const
    {
        return values.end();
    }

private:
    std::vector<Value> values;
    std::unordered_map<Key, std::size_t> indexByKey;
};
