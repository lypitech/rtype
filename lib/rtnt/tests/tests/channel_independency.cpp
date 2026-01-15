#include <gtest/gtest.h>

#include <vector>

#include "network_fixture.hpp"

namespace {

struct PacketCh1
{
    static constexpr rtnt::core::packet::Id kId = 0x2304;
    static constexpr rtnt::core::packet::Name kName = "PACKET_CH1";
    static constexpr rtnt::core::packet::ChannelId kChannel = 1;
    static constexpr rtnt::core::packet::Flag kFlag = rtnt::core::packet::Flag::kOrdered;

    uint32_t seq;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & seq;
    }
};

struct PacketCh2
{
    static constexpr rtnt::core::packet::Id kId = 0x2204;
    static constexpr rtnt::core::packet::Name kName = "PACKET_CH2";
    static constexpr rtnt::core::packet::ChannelId kChannel = 2;
    static constexpr rtnt::core::packet::Flag kFlag = rtnt::core::packet::Flag::kOrdered;

    uint32_t seq;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & seq;
    }
};

}  // namespace

TEST_F(NetworkTest,
       channel_independency)
{
    std::vector<uint32_t> receivedCh1;
    std::vector<uint32_t> receivedCh2;

    server->getPacketDispatcher().bind<PacketCh1>([&](auto, const PacketCh1& pkt) {
        LOG_INFO("Received packet on channel 1: {}", pkt.seq);
        receivedCh1.push_back(pkt.seq);
    });

    server->getPacketDispatcher().bind<PacketCh2>([&](auto, const PacketCh2& pkt) {
        LOG_INFO("Received packet on channel 2: {}", pkt.seq);
        receivedCh2.push_back(pkt.seq);
    });

    client->connect("127.0.0.1", 4242);
    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Failed to connect";

    LOG_INFO("Dropping first packet on channel 1");
    client->setSimulatedPacketLossPercentage(100);

    PacketCh1 p1_0{0};
    client->send(p1_0);  // this packet WILL be lost

    std::this_thread::sleep_for(
        std::chrono::milliseconds(50));  // decrease if ran on slow machines.

    LOG_INFO("Sending second packet on channel 1 and third packet on channel 2");
    client->setSimulatedPacketLossPercentage(0);

    PacketCh1 p1_1{1};
    client->send(p1_1);

    PacketCh2 p2_0{0};
    client->send(p2_0);

    bool ch2_received = waitFor([&]() { return receivedCh2.size() == 1; }, std::chrono::seconds(2));

    ASSERT_TRUE(ch2_received) << "Channel 2 should have processed its packet immediately!";
    EXPECT_EQ(receivedCh2[0], 0);

    EXPECT_EQ(receivedCh1.size(), 0) << "Channel 1 should be blocked waiting for Packet 0!";

    bool ch1_recovered =
        waitFor([&]() { return receivedCh1.size() == 2; }, std::chrono::seconds(5));

    ASSERT_TRUE(ch1_recovered) << "Channel 1 failed to recover missing packet.";

    EXPECT_EQ(receivedCh1[0], 0);
    EXPECT_EQ(receivedCh1[1], 1);
}
