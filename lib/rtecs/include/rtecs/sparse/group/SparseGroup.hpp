#pragma once

#include <tuple>

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
     * @tparam T The component type
     * @param entityId The entity ID
     * @param view The view
     * @param set The set
     */
    template <typename T>
    void addEntity(types::EntityID entityId, SparseView<types::EntityID, std::reference_wrapper<T>>& view,
                   std::reference_wrapper<SparseSet<T>> set)
    {
        types::OptionalRef<T> optionalComponent = set.get().get(entityId);

        if (optionalComponent.has_value()) {
            view.put(entityId, optionalComponent.value());
        } else {
            LOG_WARN("Cannot add the entity to the view: The entity with id {} do not have the component with id {}",
                     entityId, set.get().getId());
        }
    }

    /**
     * @brief A helper static method that is used to apply the callback only on the entities that have the components.
     *
     * @warning If an entity is supposed to have all the required components, but do not have one of them, then the callback will not be applied on it.
     *
     * @param entityId The entity's ID
     * @param callback The callback to apply on this entity instances
     * @param instances The instances of the entity
     */
    static void applyHelper(types::EntityID entityId, std::function<void(const types::EntityID&, Ts&...)> callback,
                            types::OptionalRef<Ts>... instances)
    {
        if ((... && instances.has_value())) {
            callback(entityId, instances.value()...);
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
    explicit SparseGroup(types::OptionalRef<SparseSet<Ts>>... sets)
    {
        std::vector<types::OptionalRef<ISparseSet>> mixedSets{sets...};

        const types::OptionalRef<ISparseSet> driver =
            *std::min_element(mixedSets.begin(), mixedSets.end(),
                              [](const types::OptionalCRef<ISparseSet> a, const types::OptionalCRef<ISparseSet> b) {
                                  if (!a.has_value() || !b.has_value()) {
                                      LOG_CRIT("A component in a group has not been registered in the ECS.");
                                  }
                                  return a.has_value() && b.has_value() && a->get().size() < b->get().size();
                              });

        if (!driver.has_value()) {
            LOG_CRIT("None of the components specified for this group have been registered.");
            return;
        }

        for (size_t entityId : driver->get().getEntities()) {
            const bool isValid = (... && (sets.has_value() && sets->get().has(entityId)));

            if (isValid) {
                std::apply([&](auto&... view) { (addEntity<Ts>(entityId, view, sets.value()), ...); }, _group);
            }
        }
    }

    /**
     * @brief Get the component instance of a specific entity from the group.
     *
     * @warning If the component do not exist in the SparseGroup, the behaviour is undefined.
     *
     * @tparam T The component type
     * @param entityId The entity ID
     * @return The component instance of the specified entity
     */
    template <typename T>
    types::OptionalRef<T> getEntity(types::EntityID entityId)
    {
        return get<T>().at(entityId);
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
        constexpr bool contains = (std::is_same_v<T, Ts> || ...);
        static_assert(contains, "Requested component type T is not part of this SparseGroup");
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

    void apply(std::function<void(const types::EntityID&, Ts&...)> callback)
    {
        for (auto entity : getEntities()) {
            std::apply(
                [&](auto&... views) {
                    applyHelper(entity, callback, views.at(entity)...);
                    // callback(entity, views.at(entity)...);
                },
                _group);
        }
    }
};

}  // namespace rtecs::sparse
