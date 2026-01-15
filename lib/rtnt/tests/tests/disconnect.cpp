#include <gtest/gtest.h>

#include "network_fixture.hpp"

TEST_F(NetworkTest,
       disconnect)
{
    bool clientDisconnected = false;

    server->onDisconnect(
        [&](const std::shared_ptr<rtnt::core::Session>&) { clientDisconnected = true; });

    client->connect("127.0.0.1", 4242);

    EXPECT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Client failed to connect.";

    client->disconnect();

    EXPECT_TRUE(waitFor([&]() { return clientDisconnected; })) << "Client failed to disconnect.";
}
