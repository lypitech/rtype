#pragma once

#include <unordered_map>
#include <vector>

namespace rtecs {

template <typename Key, typename T>
class SparseVectorView
{
private:
    std::unordered_map<Key, size_t> _map;
    std::vector<T> _vector;

public:
    explicit SparseVectorView() = default;
    ~SparseVectorView() = default;

    void emplace(Key key,
                 T value)
    {
        _vector.push_back(std::move(value));
        _map.emplace(key, _vector.size() - 1);
    }

    T &operator[](Key key)
    {
        auto [it, inserted] = _map.try_emplace(key, _vector.size());
        if (inserted) {
            _vector.emplace_back();
        }
        return _vector[it->second];
    }

    const T &operator[](Key key) const
    {
        size_t index = _map.at(key);
        return _vector[index];
    }

    std::vector<T> &getDense() { return _vector; }

    const std::vector<T> &getDense() const { return _vector; }

    size_t getDenseIndex(Key key) const { return _map.at(key); }
};

}  // namespace rtecs
