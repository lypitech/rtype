#include "network.hpp"

#include "rtecs/systems/ASystem.hpp"

namespace systems {

Network::Network(rtnt::core::Client& client,
                 service::Network& service)
    : ASystem("Network"),
      _client(client),
      _service(service)
{
}

void Network::apply(rtecs::ECS&) { _service.flush(_client); }

}  // namespace systems
