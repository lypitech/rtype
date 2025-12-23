#pragma once
#include <variant>

#include "ASystem.hpp"
#include "packets/client/join.hpp"
#include "packets/client/start.hpp"
#include "packets/client/user_input.hpp"
#include "rtnt/core/client.hpp"

namespace packet {

using Variant = std::variant<Join, Start, UserInput>;

}

namespace service {

class Network
{
public:
    template <typename T>
    void send(T packet)
    {
        _outgoingQueue.push(packet);
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

class Network : public rtecs::ASystem
{
public:
    explicit Network(rtnt::core::Client& client,
                     service::Network& service);

    void apply(rtecs::ECS& ecs) override;

private:
    rtnt::core::Client& _client;
    service::Network& _service;
};

}  // namespace systems
