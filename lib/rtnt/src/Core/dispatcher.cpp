#include "rtnt/Core/dispatcher.hpp"
#include "rtnt/Core/Packets/connect.hpp"
#include "rtnt/Core/Packets/disconnect.hpp"

namespace rtnt::core {

Dispatcher::Dispatcher()
{
    _internal_bind<packet::internal::Connect>();
    _internal_bind<packet::internal::Disconnect>();
}

void Dispatcher::dispatch(const std::shared_ptr<Session>& session, Packet& packet)
{
    auto iterator = _handlers.find(packet.getId());

    if (iterator != _handlers.end()) {
        iterator->second(session, packet);
    } else {
        LOG_ERR("Trying to dispatch an unknown packet (#{}).", packet.getId());  // todo: better logging
    }
}

}  // namespace rtnt::core
