#pragma once

#include "lobby/lobby.hpp"
#include "rtecs/systems/ASystem.hpp"

namespace server::systems {

class ApplyBulletMovement final : public rtecs::systems::ASystem
{
public:
    explicit ApplyBulletMovement(Lobby& lobby);
    void apply(rtecs::ECS& ecs) override;

private:
    Lobby& _lobby;
};

}  // namespace server::systems
