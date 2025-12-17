#include "handlers.hpp"
#include "rteng.hpp"

namespace rteng::client_side::handlers {

using Vec = std::vector<std::vector<uint8_t>>;

void handleWorldInit(const SessionPtr&,
                     const packet::WorldInit& packet)
{
    LOG_DEBUG("Handling WorldInit.");
    for (size_t i = 0; i < packet.bitsets.size(); i++) {
        const rtecs::DynamicBitSet bitset(packet.bitsets[i]);
        BindingMap& binding_map = GameEngine::getInstance().getBindingMap();
        const std::unique_ptr<rtecs::ECS>& ecs = GameEngine::getInstance().getEcs();

        if (binding_map.contains(packet.ids[i])) {
            LOG_TRACE_R3("Entity has already been created, ignoring...");
            return;
        }
        LOG_TRACE_R1("Creating new entity.");
        const rtecs::EntityID real = ecs->registerEntity(bitset);
        binding_map.emplace(packet.ids[i], real);
        GameEngine::getInstance().getFactory().apply(*ecs, real, bitset, packet.entities[i]);
    }
}

}  // namespace rteng::client_side::handlers
