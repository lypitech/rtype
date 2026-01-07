#pragma once

#include <tuple>

#include "rtecs/exceptions/EntityNotFoundException.hpp"
#include "rtecs/sparse/set/SparseSet.hpp"
#include "rtecs/sparse/view/SparseView.hpp"
#include "rtecs/types/types.hpp"

namespace rtecs::sparse {

/**
 * @brief Create a group of SparseSets.
 *
 * @tparam Ts The components type that will be stored.
 */
template <typename... Ts>
class SparseGroup
{
private:
    std::tuple<SparseView<types::EntityID, std::reference_wrapper<Ts>>...> comps;

    template <typename T>
    void addEntity(types::EntityID entityId, SparseView<types::EntityID, std::reference_wrapper<T>>& view,
                   std::unique_ptr<SparseSet<T>>& set)
    {
        OptionalRef<T> optionalComponent = set->get(entityId);

        if (optionalComponent.has_value()) {
            view.put(entityId, optionalComponent.value());
        } else {
            throw exceptions::EntityNotFoundException(entityId, set->getId());
        }
    }

public:
    /**
     * @brief Instantiate the SparseGroup with multiple SparseSets.
     *
     * @note The SparseGroup will dynamically find all entities that are presents in every of the given sets.
     *
     * @param sets The SparseSets that the SparseGroup will contain.
     */
    explicit SparseGroup(std::unique_ptr<SparseSet<Ts>>&... sets)
    {
        std::vector<ISparseSet*> mixedSets{sets.get()...};

        const ISparseSet* driver =
            *std::min_element(mixedSets.begin(), mixedSets.end(),
                              [](const ISparseSet* a, const ISparseSet* b) { return a->size() < b->size(); });

        for (size_t entityId : driver->getEntities()) {
            const bool isValid = (... && sets->has(entityId));

            if (isValid) {
                std::apply([&](auto&... view) { (addEntity<Ts>(entityId, view, sets), ...); }, comps);
            }
        }
    }

    template <typename T>
    SparseView<types::EntityID, std::reference_wrapper<T>>& get()
    {
        return std::get<SparseView<types::EntityID, std::reference_wrapper<T>>>(comps);
    }

    bool has(types::EntityID entityId) { return std::get<0>(comps).has(entityId); }
};

}  // namespace rtecs::sparse
