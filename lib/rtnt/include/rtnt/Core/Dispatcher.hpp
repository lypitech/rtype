#pragma once

#include <memory>
#include <functional>

#include "Packet.hpp"

namespace rtnt::core
{

class Server;
class Client;

class Dispatcher final
{
using PacketHandler = std::function<void(const std::shared_ptr<Session>&, Packet&)>;

public:
    Dispatcher();

    template <typename T>
    void bind(std::function<void(const std::shared_ptr<Session>&, const T&)> callback)
    {
        packet::verifyUserPacketData<T>();
        registerHandler<T>(callback);
    }

    template <typename T>
    void bind()
    {
        packet::verifyInternalPacketData<T>();
        bind<T>(T::onReceive);
    }

    /**
     * @brief   Routes an incoming packet to the correct handler.
     */
    void dispatch(const std::shared_ptr<Session>& session, Packet& packet)
    {
        auto it = _handlers.find(packet.getId());

        if (it != _handlers.end()) {
            it->second(session, packet);
        } else {
            LOG_ERR("Trying to dispatch an unknown packet (#{}).", packet.getId()); // todo: better logging
        }
    }

private:
    friend class Server;
    friend class Client;

    std::unordered_map<packet::Id, PacketHandler> _handlers;

    template <typename T>
    void internal_bind(std::function<void(const std::shared_ptr<Session>&, const T&)> callback)
    {
        packet::verifyPacketData<T>();
        registerHandler<T>(callback);
    }

    template <typename T>
    void internal_bind()
    {
        packet::verifyInternalPacketData<T>();
        internal_bind<T>(T::onReceive);
    }

    template <typename T>
    void registerHandler(std::function<void(const std::shared_ptr<Session>&, const T&)> callback)
    {
        packet::Id packetId = T::kId;

        if (_handlers.contains(packetId)) {
            LOG_CRIT(
                "Error binding Packet #{}: ID is already registered.",
                packetId
            );
            // todo: throw exception?
            return;
        }

        _handlers[packetId] = [callback](std::shared_ptr<Session> session, Packet& packet) {
            T data;
            try {
                packet >> data;
            } catch (const std::exception& e) {
                LOG_CRIT("Could not parse packet: {}", e.what()); // todo: better logging
                return;
            }
            callback(session, data);
        };
    }
};

}
