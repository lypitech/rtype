#include <gtest/gtest.h>

#include "network_fixture.hpp"

TEST_F(NetworkTest,
       reconnection)
{
    bool clientConnected = false;

    client->onConnect([&]() { clientConnected = true; });
    client->connect("127.0.0.1", 4242);

    EXPECT_TRUE(waitFor([&]() { return clientConnected; }, std::chrono::seconds(10)))
        << "Client failed to connect.";
}
