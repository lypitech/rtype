#pragma once

#include <unordered_map>
#include <vector>

#include "logger/Logger.h"

namespace rtecs::sparse {

template <typename Key, typename T>
class SparseVectorView
{
private:
    std::unordered_map<Key, size_t> _keyToIndex;
    std::vector<T> _values;
    std::vector<Key> _indexToKey;
    T _defaultValue;

public:
    /**
     * Instantiate a new SparseVectorView.
     *
     * @param defaultValue The default value of the SparseVectorView
     */
    explicit SparseVectorView(T defaultValue = {})
        : _defaultValue(defaultValue) {};

    /**
     * Destroy a SparseVectorView.
     */
    ~SparseVectorView() = default;

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
     * @note If the key is not found, then a value corresponding to this key is created.
     *
     * @param key The key of the value to access.
     * @return A reference to the corresponding value.
     */
    T &operator[](Key key)
    {
        auto [it, inserted] = _keyToIndex.try_emplace(key, _values.size());
        if (inserted) {
            _values.emplace_back(_defaultValue);
        }
        return _values[it->second];
    }

    /**
     * @brief Access to the const-reference of a value.
     *
     * @important If the key is not found, then the behaviour is unknown.
     *
     * @param key The key of the value to access.
     * @return A const-reference to the corresponding value.
     */
    const T &operator[](Key key) const
    {
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
     * @brief Get the index of the value in the vector container.
     *
     * @param key The key of the value.
     * @return The index of the value in the vector container.
     */
    size_t getIndex(Key key) const { return _keyToIndex.at(key); }
};

}  // namespace rtecs::sparse
