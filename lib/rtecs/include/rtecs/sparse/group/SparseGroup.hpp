#pragma once

#include <tuple>

#include "rtecs/exceptions/EntityNotFoundException.hpp"
#include "rtecs/sparse/set/SparseSet.hpp"
#include "rtecs/sparse/view/SparseView.hpp"
#include "rtecs/types/types.hpp"

namespace rtecs::sparse {

/**
 * @brief This class groups all the entities that has all the specified components in a single group.
 *
 * @tparam Ts The components type that will be stored.
 */
template <typename... Ts>
class SparseGroup
{
public:
    template <typename T>
    using View = SparseView<types::EntityID, std::reference_wrapper<T>>;

private:
    std::tuple<View<Ts>...> _group;

    /**
     * @brief Add an entity component instance from the set to the view.
     *
     * @throw exceptions::EntityNotFoundException If the entity does not exist.
     *
     * @tparam T The component type
     * @param entityId The entity ID
     * @param view The view
     * @param set The set
     */
    template <typename T>
    void addEntity(types::EntityID entityId, SparseView<types::EntityID, std::reference_wrapper<T>>& view,
                   std::reference_wrapper<SparseSet<T>> set)
    {
        OptionalRef<T> optionalComponent = set.get().get(entityId);

        if (optionalComponent.has_value()) {
            view.put(entityId, optionalComponent.value());
        } else {
            throw exceptions::EntityNotFoundException(entityId, set.get().getId());
        }
    }

public:
    /**
     * @brief Instantiate the SparseGroup with multiple SparseSets.
     *
     * @note The SparseGroup will dynamically find all entities that are presents in every of the given sets.
     *
     * @throw exceptions::EntityNotFoundException This error should not be thrown. If this happens, create an issue on the Githuv repository of this project.
     *
     * @param sets The SparseSets that the SparseGroup will contain.
     */
    explicit SparseGroup(SparseSet<Ts>&... sets)
    {
        std::vector<std::reference_wrapper<ISparseSet>> mixedSets{sets...};

        const std::reference_wrapper<ISparseSet> driver = *std::min_element(
            mixedSets.begin(), mixedSets.end(),
            [](const std::reference_wrapper<ISparseSet> a, const std::reference_wrapper<ISparseSet> b) {
                return a.get().size() < b.get().size();
            });

        for (size_t entityId : driver.get().getEntities()) {
            const bool isValid = (... && sets.has(entityId));

            if (isValid) {
                std::apply([&](auto&... view) { (addEntity<Ts>(entityId, view, sets), ...); }, _group);
            }
        }
    }

    /**
     * @brief Get the component instance of a specific entity from the group.
     *
     * @throw std::out_of_range If the entity does not exist in the group.
     *
     * @tparam T The component type
     * @param entityId The entity ID
     * @return The component instance of the specified entity
     */
    template <typename T>
    std::reference_wrapper<T> getEntity(types::EntityID entityId)
    {
        return get<T>()[entityId];
    }

    /**
     * @brief Get the entities' ID contained in this group.
     *
     * @return The entities' ID contained in this group.
     */
    const std::vector<types::EntityID>& getEntities() { return std::get<0>(_group).getKeys(); }

    /**
     * @brief Get all the instances of a specific component type from the group.
     *
     * @tparam T The component type
     * @return A SparseView of the entities
     */
    template <typename T>
    SparseView<types::EntityID, std::reference_wrapper<T>>& get()
    {
        return std::get<SparseView<types::EntityID, std::reference_wrapper<T>>>(_group);
    }

    /**
     * @brief Get all the SparseView stored in the group.
     *
     * @return All the SparseView stored in the group.
     */
    std::tuple<SparseView<types::EntityID, std::reference_wrapper<Ts>>...>& getAll() { return _group; }

    /**
     * @brief Check if the group has an entity.
     *
     * @param entityId The entity ID
     * @return `true` if the group has the specified entity, `false` otherwise
     */
    bool has(types::EntityID entityId) { return std::get<0>(_group).has(entityId); }

    void apply(std::function<void(Ts&...)> callback)
    {
        for (auto entity : getEntities()) {
            std::apply([&](auto&... views) { callback(views[entity].get()...); }, _group);
        }
    }

    void _apply(std::function<void(Ts&...)> callback)
    {
        // for (auto entity : getEntities()) {
        //     std::apply([&](auto&... views) {
        //         callback((views[entity].get())...);
        //     }, _group);
        // }
    }
};

}  // namespace rtecs::sparse
