#include <gtest/gtest.h>

#include "network_fixture.hpp"

TEST_F(NetworkTest,
       reconnection)
{
    bool clientConnected = false;

    server->setSimulatedPacketLossPercentage(100);

    client->onConnect([&]() { clientConnected = true; });
    client->connect("127.0.0.1", 4242);

    auto start = std::chrono::steady_clock::now();
    bool networkRepaired = false;

    EXPECT_TRUE(waitFor(
        [&]() {
            if (!networkRepaired) {
                auto now = std::chrono::steady_clock::now();
                if (now - start > std::chrono::seconds(5)) {
                    server->setSimulatedPacketLossPercentage(0);
                    networkRepaired = true;
                }
            }

            return clientConnected;
        },
        std::chrono::seconds(15)))
        << "Client failed to connect after network recovery.";
}
