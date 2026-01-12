#include <gtest/gtest.h>

#include "network_fixture.hpp"

namespace {

struct Example
{
    static constexpr rtnt::core::packet::Id kId = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";
};

}  // namespace

TEST_F(NetworkTest,
       empty_packet)
{
    size_t packetSize = 1801;

    server->onMessage([&](const std::shared_ptr<rtnt::core::Session>&, rtnt::core::Packet& p) {
        if (p.getId() == Example::kId) {
            packetSize = p.getPayload().size();
        }
    });

    client->connect("127.0.0.1", 4242);

    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Client failed to connect.";

    Example ex{};
    client->send(ex);

    EXPECT_TRUE(waitFor([&]() { return packetSize == 0; }))
        << "Server has received no EXAMPLE packet.";
}
