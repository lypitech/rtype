#pragma once

#include "lobby/lobby.hpp"
#include "rtecs/ECS.hpp"
#include "rtecs/systems/ASystem.hpp"

namespace server::systems {

class BroadcastDeadEntities final : public rtecs::systems::ASystem
{
private:
    Lobby &_lobby;

public:
    explicit BroadcastDeadEntities(Lobby &lobby);
    void apply(rtecs::ECS &ecs) override;
};

}  // namespace server::systems
