#include <gtest/gtest.h>

#include <numeric>

#include "network_fixture.hpp"

namespace {

struct Example
{
    static constexpr rtnt::core::packet::Id kId = 1001;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";
    static constexpr rtnt::core::packet::Flag kFlag = rtnt::core::packet::Flag::kReliable;

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

    std::vector<uint32_t> receivedIndices;
    bool hasDisconnected = false;

    server->getPacketDispatcher().bind<Example>([&](const auto& /*session*/, const Example& pkt) {
        LOG_DEBUG("Received packet, with body: [{}]", pkt.x);
        receivedIndices.push_back(pkt.x);
    });

    client->onConnect([&]() { client->setSimulatedPacketLossPercentage(65); });
    client->onDisconnect([&]() { hasDisconnected = true; });

    client->connect("127.0.0.1", 4242);
    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); }, std::chrono::seconds(10)))
        << "Client failed to connect.";

    auto before = std::chrono::steady_clock::now();

    for (uint32_t i = 1; i <= packetAmount; ++i) {
        Example pkt{.x = i};
        LOG_DEBUG("Sending packet with body: [{}]", i);
        client->send(pkt);
    }

    bool finished =
        waitFor([&]() { return receivedIndices.size() >= packetAmount || hasDisconnected; },
                std::chrono::seconds(50));

    auto after = std::chrono::steady_clock::now();

    ASSERT_FALSE(hasDisconnected) << "Client disconnected unexpectedly.";

    ASSERT_TRUE(finished) << "Timeout: Only received " << receivedIndices.size() << "/"
                          << packetAmount << " packets.";

    // fixme: doesnt work on g++-14, removing because flemme
    // LOG_INFO("Final list: {}", receivedIndices);
    LOG_INFO("Took {}", after - before);

    std::ranges::sort(receivedIndices);

    std::vector<uint32_t> expectedIndices(packetAmount);
    std::iota(expectedIndices.begin(), expectedIndices.end(), 1);

    EXPECT_EQ(receivedIndices, expectedIndices)
        << "Mismatch! The received packets do not follow the sequence 1..181 perfectly.";

    if (receivedIndices != expectedIndices) {
        auto it = std::ranges::unique(receivedIndices).begin();
        bool hasDuplicates = it != receivedIndices.end();
        if (hasDuplicates) {
            LOG_ERR("Error: Duplicate packets were received!");
        }
    }
}
