#include <gtest/gtest.h>

#include "network_fixture.hpp"

namespace {

struct Example
{
    static constexpr rtnt::core::packet::Id kId = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";
    static constexpr rtnt::core::packet::Flag kFlag = rtnt::core::packet::Flag::kOrdered;

    uint32_t x;

    bool operator==(const Example&) const = default;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & x;
    }
};

}  // namespace

TEST_F(NetworkTest,
       packet_loss)
{
    const uint32_t packetAmount = 181;

    client->setSimulatedPacketLossPercentage(75);

    std::mutex dataMutex;
    std::vector<uint32_t> receivedIndices;

    server->getPacketDispatcher().bind<Example>([&](const auto& /*session*/, const Example& pkt) {
        std::lock_guard lock(dataMutex);
        LOG_INFO("Received packet, with body: [{}]", pkt.x);
        receivedIndices.push_back(pkt.x);
    });

    client->connect("127.0.0.1", 4242);
    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); }, std::chrono::seconds(10)))
        << "Client failed to connect.";

    for (uint32_t i = 0; i < packetAmount; ++i) {
        Example pkt{.x = i};
        LOG_INFO("Sending packet with body: [{}]", i);
        client->send(pkt);
    }

    bool finished = waitFor(
        [&]() {
            std::lock_guard lock(dataMutex);
            return receivedIndices.size() >= packetAmount;
        },
        std::chrono::seconds(50));

    ASSERT_TRUE(finished) << "Timeout: Only received " << receivedIndices.size() << "/"
                          << packetAmount << " packets.";

    std::lock_guard lock(dataMutex);

    ASSERT_EQ(receivedIndices.size(), packetAmount);

    LOG_INFO("Final list: {}", receivedIndices);

    for (uint32_t i = 0; i < packetAmount; ++i) {
        ASSERT_EQ(receivedIndices[i], i)
            << "Ordering failed! Index " << i << " was not the expected value.";
    }
}
