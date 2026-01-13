#include <gtest/gtest.h>

#include "network_fixture.hpp"

namespace {

struct Example
{
    static constexpr rtnt::core::packet::MessageId kId = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    std::string str;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & str;
    }
};

}  // namespace

TEST_F(NetworkTest,
       string_packet)
{
    std::string strToTransmit{"hi lol"};
    std::string receivedString{};

    server->getPacketDispatcher().bind<Example>(
        [&](const auto&, const Example& pkt) { receivedString = pkt.str; });

    client->connect("127.0.0.1", 4242);

    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Client failed to connect.";

    Example ex{.str = strToTransmit};
    client->send(ex);

    ASSERT_TRUE(waitFor([&]() { return !receivedString.empty(); }))
        << "Server has received no EXAMPLE packet.";

    EXPECT_EQ(receivedString, strToTransmit);
}
