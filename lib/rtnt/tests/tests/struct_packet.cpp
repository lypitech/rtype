#include <gtest/gtest.h>

#include "network_fixture.hpp"

namespace {

struct Body
{
    int x, y, z;
    std::string s;

    bool operator==(const Body&) const = default;

    // Not putting this will cause compilation to fail (in Example::serialize)
    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & x & y & z & s;
    }
};

struct Example
{
    static constexpr rtnt::core::packet::MessageId kId = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    Body body;

    bool operator==(const Example&) const = default;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & body;
    }
};

}  // namespace

TEST_F(NetworkTest,
       struct_packet)
{
    Body dataToTransmit{13, 6, 8, "are you gonna leave me now"};
    std::optional<Body> receivedData;

    server->getPacketDispatcher().bind<Example>(
        [&](const auto&, const Example& pkt) { receivedData = pkt.body; });

    client->connect("127.0.0.1", 4242);

    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Client failed to connect.";

    Example ex{.body = dataToTransmit};
    client->send(ex);

    ASSERT_TRUE(waitFor([&]() { return receivedData.has_value(); }))
        << "Server has received no EXAMPLE packet.";

    EXPECT_EQ(receivedData, dataToTransmit);
}
