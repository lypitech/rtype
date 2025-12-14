#include <gtest/gtest.h>

#include "network_fixture.hpp"

namespace {

struct Example
{
    static constexpr rtnt::core::packet::Id kId = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    std::vector<uint8_t> data;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & data;
    }
};

}  // namespace

TEST_F(NetworkTest, vector_packet)
{
    std::vector<uint8_t> dataToTransmit{ 13, 6, 8, 23, 4, 68 };
    std::vector<uint8_t> receivedData{};

    server->getPacketDispatcher().bind<Example>([&](const auto&, const Example& pkt) { receivedData = pkt.data; });

    client->connect("127.0.0.1", 4242);

    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Client failed to connect.";

    Example ex{.data = dataToTransmit};
    client->send(ex);

    ASSERT_TRUE(waitFor([&]() { return !receivedData.empty(); })) << "Server has received no EXAMPLE packet.";

    EXPECT_EQ(receivedData, dataToTransmit);
}
