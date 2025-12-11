#pragma once

#include <memory>
#include <functional>

#include "Packet.hpp"

namespace rtnt::core
{

class Server;
class Client;

/**
 * @class   Dispatcher
 * @brief   Router for incoming packets
 *
 * The Dispatcher class is responsible for dispatching incoming packets to their respective
 * handler.
 *
 * Handlers must be bound with the @code bind@endcode function. Everything else is automatically
 * managed by Dispatcher, Server and Client classes.
 */
class Dispatcher final
{

using PacketHandler = std::function<void(const std::shared_ptr<Session>&, Packet&)>;

public:
    Dispatcher();

    /**
     * @brief   Binds a packet type to a certain callback.
     * @tparam  T           Type of packet to bind
     * @param   callback    Function to call whenever a packet of type T is received
     */
    template <typename T>
    void bind(std::function<void(const std::shared_ptr<Session>&, const T&)> callback)
    {
        packet::verifyUserPacketData<T>();
        registerHandler<T>(callback);
    }

    /**
     * @brief   Binds a packet type to a certain callback.
     *
     * In this overload, callback is automatically resolved from the Packet struct.
     *
     * @tparam  T   Type of packet to bind
     */
    template <typename T>
    void bind()
    {
        packet::verifyPacketIntegratedCallback<T>();
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
    void _internal_bind(std::function<void(const std::shared_ptr<Session>&, const T&)> callback)
    {
        packet::verifyInternalPacketData<T>();
        registerHandler<T>(callback);
    }

    template <typename T>
    void _internal_bind()
    {
        packet::verifyPacketIntegratedCallback<T>();
        _internal_bind<T>(T::onReceive);
    }

    /**
     * @brief   Binds a packet type to a certain callback.
     * @tparam  T           Type of packet to register
     * @param   callback    Function to call whenever a packet of type T is received
     */
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
