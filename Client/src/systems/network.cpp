#include "network.hpp"

namespace systems {

Network::Network(rtnt::core::Client& client,
                 service::Network& service)
    : ASystem(rtecs::DynamicBitSet()),
      _client(client),
      _service(service)
{
}

void Network::apply(rtecs::ECS&) { _service.flush(_client); }

}  // namespace systems
