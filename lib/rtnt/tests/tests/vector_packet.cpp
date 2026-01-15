#include <gtest/gtest.h>

#include "network_fixture.hpp"

namespace {

struct Example
{
    static constexpr rtnt::core::packet::Id kId = 1001;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    std::vector<uint8_t> data;
    std::vector<uint8_t> data2;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & data & data2;
    }
};

}  // namespace

TEST_F(NetworkTest,
       vector_packet)
{
    std::vector<uint8_t> dataToTransmit1{13, 6, 8};
    std::vector<uint8_t> dataToTransmit2{23, 4, 68, 28, 1, 60, 4, 9, 3};
    std::vector<uint8_t> receivedData1{};
    std::vector<uint8_t> receivedData2{};

    server->getPacketDispatcher().bind<Example>([&](const auto&, const Example& pkt) {
        receivedData1 = pkt.data;
        receivedData2 = pkt.data2;
    });

    client->connect("127.0.0.1", 4242);

    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Client failed to connect.";

    Example ex{.data = dataToTransmit1, .data2 = dataToTransmit2};
    client->send(ex);

    ASSERT_TRUE(waitFor([&]() { return !receivedData1.empty() && !receivedData2.empty(); }))
        << "Server has received no EXAMPLE packet.";

    EXPECT_EQ(receivedData1, dataToTransmit1);
    EXPECT_EQ(receivedData2, dataToTransmit2);
}
