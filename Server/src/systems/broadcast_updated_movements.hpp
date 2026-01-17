#pragma once

#include "lobby/lobby.hpp"
#include "rtecs/ECS.hpp"
#include "rtecs/systems/ASystem.hpp"

namespace server::systems {

class BroadcastUpdatedMovements final : public rtecs::systems::ASystem
{
private:
    Lobby &_lobby;

public:
    explicit BroadcastUpdatedMovements(Lobby &lobby);
    void apply(rtecs::ECS &ecs) override;
};

}  // namespace server::systems
