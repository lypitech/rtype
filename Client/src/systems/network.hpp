#pragma once
#include <variant>

#include "packets/client/join.hpp"
#include "packets/client/lobby_list.hpp"
#include "packets/client/start.hpp"
#include "packets/client/user_input.hpp"
#include "rtecs/systems/ASystem.hpp"
#include "rtnt/core/client.hpp"

namespace packet {

using Variant = std::variant<Join, Start, UserInput, LobbyList>;

}

namespace service {

/**
 * @class service::Network
 * @brief A queue for outgoing packets.
 *
 */
class Network final
{
public:
    template <typename T>
    void send(T packet)
    {
        _outgoingQueue.emplace(packet);
    }

    void flush(rtnt::core::Client& client)
    {
        while (!_outgoingQueue.empty()) {
            packet::Variant packet = _outgoingQueue.front();
            _outgoingQueue.pop();
            std::visit([&client](auto&& p) { client.send(p); }, packet);
        }
    };

private:
    std::queue<packet::Variant> _outgoingQueue;
};
}  // namespace service

namespace systems {

class Network final : public rtecs::systems::ASystem
{
public:
    /**
     * @brief A network system to send packets within the ecs.
     * @param client A reference to a @code rtnt::Client@endcode
     * @param service A reference to the @code service::Network@endcode given to the other systems.
     */
    explicit Network(rtnt::core::Client& client,
                     service::Network& service);

    void apply(rtecs::ECS& ecs) override;

private:
    rtnt::core::Client& _client;  ///< The client used to send packets.
    service::Network& _service;   ///< The queue used to store outgoing packets.
};

}  // namespace systems
