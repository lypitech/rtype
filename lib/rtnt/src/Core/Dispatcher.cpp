#include "rtnt/Core/Dispatcher.hpp"

#include "rtnt/Core/Packets/Connect.hpp"

namespace rtnt::core
{

Dispatcher::Dispatcher()
{
    _internal_bind<packet::internal::Connect>();
}

}
