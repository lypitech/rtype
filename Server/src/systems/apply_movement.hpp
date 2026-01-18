#pragma once

#include <optional>

#include "components/hitbox.hpp"
#include "components/position.hpp"
#include "components/velocity.hpp"
#include "rtecs/sparse/group/SparseGroup.hpp"
#include "rtecs/systems/ASystem.hpp"
#include "rtecs/types/types.hpp"

namespace server::systems {

class ApplyMovement final : public rtecs::systems::ASystem
{
private:
    /**
     * @brief Check if a horizontal/vertical collision is detected.
     *
     * @param refPos The reference position
     * @param refBox The reference hitbox
     * @param otherPos The other position
     * @param otherBox The other hitbox
     * @return `true` if a collision is detected, `false` otherwise.
     */
    static bool collide(const components::Position &refPos,
                        const components::Hitbox &refBox,
                        const components::Position &otherPos,
                        const components::Hitbox &otherBox);

    static void handleHorizontalMovement(rtecs::types::EntityID id,
                                         components::Position &pos,
                                         const components::Velocity &vel,
                                         const components::Hitbox &box,
                                         rtecs::sparse::SparseGroup<components::Position,
                                                                    components::Hitbox> &colliders);
    static void handleVerticalMovement(rtecs::types::EntityID id,
                                       components::Position &pos,
                                       const components::Velocity &vel,
                                       const components::Hitbox &box,
                                       rtecs::sparse::SparseGroup<components::Position,
                                                                  components::Hitbox> &colliders);

public:
    explicit ApplyMovement();
    void apply(rtecs::ECS &ecs) override;
};

}  // namespace server::systems
