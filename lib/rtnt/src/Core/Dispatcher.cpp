#include "rtnt/Core/Dispatcher.hpp"

#include "rtnt/Core/Packets/Connect.hpp"

namespace rtnt::core
{

Dispatcher::Dispatcher()
{
    internal_bind<packet::internal::Connect>();
}

}
