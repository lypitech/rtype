#include <gtest/gtest.h>

#include "network_fixture.hpp"

TEST_F(NetworkTest, handshake)
{
    bool clientConnected = false;

    client->onConnect([&]() { clientConnected = true; });
    client->connect("127.0.0.1", 4242);

    EXPECT_TRUE(waitFor([&]() { return clientConnected; })) << "Client failed to connect.";
}
