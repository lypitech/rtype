#pragma once

#include <unordered_map>
#include <vector>

#include "logger/Logger.h"
#include "rtecs/types/types.hpp"

namespace rtecs::sparse {

template <typename Key, typename T>
class SparseView
{
private:
    std::unordered_map<Key, size_t> _keyToIndex;
    std::vector<T> _values;
    std::vector<Key> _indexToKey;

public:
    /**
     * Instantiate a new SparseView.
     */
    explicit SparseView() = default;

    /**
     * Destroy a SparseView.
     */
    ~SparseView() = default;

    /**
     * @brief Insert / Overwrite a value.
     *
     * @param key The key of the value to insert.
     * @param value The value to insert.
     */
    void put(Key key, T value)
    {
        _values.push_back(std::move(value));
        _keyToIndex.emplace(key, _values.size() - 1);
        _indexToKey.push_back(key);
    }

    /**
     * @brief Remove a value.
     *
     * @note If the value does not exist, nothing happen.
     *
     * @param key The key of the value to erase.
     */
    void erase(Key key)
    {
        if (_keyToIndex.contains(key)) {
            size_t index = _keyToIndex[key];
            Key lastKey = _indexToKey[_values.size() - 1];
            std::swap(_keyToIndex[key], _keyToIndex[lastKey]);

            T &value = _values[index];
            T &lastValue = _values[_values.size() - 1];
            std::swap(value, lastValue);

            _values.pop_back();
            _indexToKey.pop_back();
            _keyToIndex.erase(key);
        }
    }

    /**
     * @brief Check if a key exist in the view.
     *
     * @param key The key to check for.
     * @return `true` if the key exists, `false` otherwise.
     */
    bool has(Key key) const { return _keyToIndex.contains(key); }

    /**
     * @brief Access to the reference of a value.
     *
     * @param key The key of the value to access.
     * @return A reference to the corresponding value.
     */
    types::OptionalRef<T> at(Key key)
    {
        if (!_keyToIndex.contains(key)) {
            return std::nullopt;
        }

        size_t index = _keyToIndex.at(key);
        return _values[index];
    }

    /**
     * @brief Access to the const-reference of a value.
     *
     * @param key The key of the value to access.
     * @return A const-reference to the corresponding value.
     */
    types::OptionalCRef<T> at(Key key) const
    {
        if (!_keyToIndex.contains(key)) {
            return std::nullopt;
        }

        size_t index = _keyToIndex.at(key);
        return _values[index];
    }

    /**
     * @brief Get a reference of the vector container.
     *
     * @return The reference of the values vector container.
     */
    std::vector<T> &getValues() { return _values; }

    /**
     * @brief Get a const-reference of the vector container.
     *
     * @return The const-reference of the values vector container.
     */
    const std::vector<T> &getValues() const { return _values; }

    /**
     * @brief Get a const-reference of the keys.
     *
     * @return The const-reference of the keys.
     */
    const std::vector<Key> &getKeys() const { return _indexToKey; }

    /**
     * @brief Get the index of the value in the vector container.
     *
     * @param key The key of the value.
     * @return The index of the value in the vector container.
     */
    size_t getIndex(Key key) const { return _keyToIndex.at(key); }
};

}  // namespace rtecs::sparse
