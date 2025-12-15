#pragma once

#include <limits>
#include <tuple>

#include "SparseSet.hpp"

namespace rtecs {

/**
 * @brief A class to iterate over entities containing a set of components.
 *
 * All component at a given index are related to the same entity.
 *
 * @tparam Components An array of component types for the entity to have.
 */
template <typename... Components>
class View
{
public:
    using SetsTuple = std::tuple<SparseSet<Components>*...>;
    /**
     * @brief An iterator to permit structured bindings.
     */
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::tuple<Components&...>;
        using pointer = value_type*;
        using reference = value_type;

        Iterator(SetsTuple sets, const std::vector<size_t>* entities, size_t index)
            : _sets(sets), _entities(entities), _index(index)
        {
            if (_index < _entities->size() && !valid()) {
                ++(*this);
            }
        }

        Iterator& operator++()
        {
            do {
                _index++;
            } while (_index < _entities->size() && !valid());
            return *this;
        }

        bool operator!=(const Iterator& other) const { return _index != other._index; }

        value_type operator*() const
        {
            size_t entityId = (*_entities)[_index];
            return std::tuple<Components&...>(getComponent<Components>(entityId)...);
        }

    private:
        SetsTuple _sets;
        const std::vector<size_t>* _entities;
        size_t _index;

        /**
         * @brief Checks all entity at index @code _index@endcode has the researched components
         * @return The boolean value corresponding.
         */
        [[nodiscard]] bool valid() const
        {
            size_t entityId = (*_entities)[_index];
            return (std::get<SparseSet<Components>*>(_sets)->has(entityId) && ...);
        }

        /**
         * @brief
         * @tparam T The component type to search for.
         * @param entityId The id of the entity supposed to have the component.
         * @returns The component instance corresponding to the search.
         */
        template <typename T>
        T& getComponent(size_t entityId) const
        {
            return std::get<SparseSet<T>*>(_sets)->get(entityId).value().get();
        }
    };

    /**
     * @brief Creates a view for the entity having all @code sets@endcode components.
     * @param sets The set of component that the entity should have.
     */
    explicit View(SparseSet<Components>&... sets)
        : _sets(&sets...)
    {
        _smallestSetIndex = findSmallestIndex(sets...);
    }

    Iterator begin() { return Iterator(_sets, getEntitiesVector(_smallestSetIndex), 0); }

    Iterator end()
    {
        const auto* entities = getEntitiesVector(_smallestSetIndex);
        return Iterator(_sets, entities, entities->size());
    }

private:
    SetsTuple _sets;           ///< A tuple of component sparseSets.
    size_t _smallestSetIndex;  ///< The set having the smallest index.

    const std::vector<size_t>* getEntitiesVector(size_t index)
    {
        const std::vector<size_t>* result = nullptr;
        size_t current = 0;

        (
            [&] {
                if (current++ == index) {
                    result = &std::get<SparseSet<Components>*>(_sets)->getEntities();
                }
            }(),
            ...);

        return result;
    }

    static size_t findSmallestIndex(SparseSet<Components>&... sets)
    {
        size_t minSize = std::numeric_limits<size_t>::max();
        size_t minIndex = 0;
        size_t currentIndex = 0;

        auto check = [&](auto& set) {
            if (set.getEntities().size() < minSize) {
                minSize = set.getEntities().size();
                minIndex = currentIndex;
            }
            currentIndex++;
        };

        (check(sets), ...);
        return minIndex;
    }
};

}  // namespace rtecs
