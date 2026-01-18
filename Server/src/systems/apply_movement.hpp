#pragma once

#include <optional>

#include "components/hitbox.hpp"
#include "components/position.hpp"
#include "components/state.hpp"
#include "components/type.hpp"
#include "components/velocity.hpp"
#include "rtecs/sparse/group/SparseGroup.hpp"
#include "rtecs/systems/ASystem.hpp"
#include "rtecs/types/types.hpp"

using namespace components;

namespace server::systems {

class ApplyMovement final : public rtecs::systems::ASystem
{
private:
    using Collider = std::tuple<Position&, Hitbox&, State&, Type&>;
    /**
     * @brief Check if a horizontal/vertical collision is detected.
     *
     * @param refPos The reference position
     * @param refBox The reference hitbox
     * @param otherPos The other position
     * @param otherBox The other hitbox
     * @return `true` if a collision is detected, `false` otherwise.
     */
    static bool collide(const Position& refPos,
                        const Hitbox& refBox,
                        const Position& otherPos,
                        const Hitbox& otherBox);

    /**
     * @bief Find a collider
     *
     * @param id The id of the moving entity
     * @param pos The position of the moving entity
     * @param box The box of the moving entity
     * @param colliders The list of colliders
     * @param expectedColliderType The excpected collider type
     * @return An optional collider if any.
     */
    static std::optional<Collider> findCollider(rtecs::types::EntityID id,
                                                const Position& pos,
                                                const Hitbox& box,
                                                rtecs::sparse::SparseGroup<Position,
                                                                           Hitbox,
                                                                           State,
                                                                           Type>& colliders,
                                                entity::Type expectedColliderType);

    /**
     * @brief Handle the movement of the bullets and enemies
     *
     * @param pos The position of the moving entity
     * @param vel The velocity of the moving entity
     * @param box The hitbox of the moving entity
     * @param state The state of the moving entity
     * @param collider The optional collider if any found
     */
    static void handleEntityMovement(Position& pos,
                                     Velocity& vel,
                                     const Hitbox& box,
                                     State& state,
                                     const std::optional<Collider>& collider);

    /**
     * @brief Handle the horizontal movement of the player
     *
     * @param pos The position of the player
     * @param vel The velocity of the player
     * @param box The hitbox of the player
     * @param collider The optional collider (which is a player) if any found
     */
    static void handlePlayerHorizontalMovement(Position& pos,
                                               Velocity& vel,
                                               const Hitbox& box,
                                               const std::optional<Collider>& collider);

    /**
     * @brief Handle the vertical movement of the player
     *
     * @param pos The position of the player
     * @param vel The velocity of the player
     * @param box The hitbox of the player
     * @param collider The optional collider (which is a player) if any found
     */
    static void handlePlayerVerticalMovement(Position& pos,
                                             Velocity& vel,
                                             const Hitbox& box,
                                             const std::optional<Collider>& collider);

public:
    explicit ApplyMovement();
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace server::systems
