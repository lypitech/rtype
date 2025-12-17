#include "handlers.hpp"
#include "rteng.hpp"

namespace rteng::client_side::handlers {

void handleSpawn(const SessionPtr&,
                 const packet::Spawn& packet)
{
    const rtecs::DynamicBitSet bitset(packet.bitmask);
    BindingMap& binding_map = GameEngine::getInstance().getBindingMap();
    const std::unique_ptr<rtecs::ECS>& ecs = GameEngine::getInstance().getEcs();

    if (binding_map.contains(packet.id)) {
        LOG_TRACE_R3("Entity has already been created, ignoring...");
        return;
    }
    const rtecs::EntityID real = ecs->registerEntity(bitset);
    binding_map.emplace(packet.id, real);
    GameEngine::getInstance().getFactory().apply(*ecs, real, bitset, packet.content);
}

}  // namespace rteng::client_side::handlers
