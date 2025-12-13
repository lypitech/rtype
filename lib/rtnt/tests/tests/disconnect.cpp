#include <gtest/gtest.h>

#include <asio/io_context.hpp>
#include <future>
#include <thread>

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "rtnt/core/client.hpp"
#include "rtnt/core/server.hpp"

TEST(rtnt, disconnect)
{
    asio::io_context context;
    auto workGuard = asio::make_work_guard(context);

    LOG_DEBUG("Initializing server and client");
    rtnt::core::Server server(context, 4242);
    rtnt::core::Client client(context);

    std::promise<bool> clientConnectedPromise;
    std::future<bool> clientConnectedFuture = clientConnectedPromise.get_future();

    std::promise<bool> serverDetectedDisconnectPromise;
    std::future<bool> serverDetectedDisconnectFuture = serverDetectedDisconnectPromise.get_future();

    server.onConnect([](const std::shared_ptr<rtnt::core::Session>& session) {
        LOG_DEBUG("Server: New session ({}) connected from {}", session->getId(), session->getEndpoint().port());
    });

    server.onDisconnect([&](const std::shared_ptr<rtnt::core::Session>& session) {
        LOG_INFO("Server: Session disconnected (Endpoint: {})", session->getEndpoint().port());
        serverDetectedDisconnectPromise.set_value(true);
    });

    client.onConnect([&]() {
        LOG_INFO("Client: Handshake complete, connected to server.");
        clientConnectedPromise.set_value(true);
    });

    server.start();

    std::thread ioThread([&context]() {
        logger::setThreadLabel("IoThread");
        context.run();
    });

    client.connect("127.0.0.1", 4242);

    ASSERT_EQ(clientConnectedFuture.wait_for(std::chrono::seconds(3)), std::future_status::ready)
        << "Test Failed: Client did not connect within timeout.";

    EXPECT_TRUE(clientConnectedFuture.get()) << "Client failed to connect.";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    LOG_INFO("Test: Triggering client disconnect...");
    client.disconnect();

    auto startTime = std::chrono::steady_clock::now();
    bool disconnectedEventReceived = false;

    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(3)) {
        server.update();
        client.update();

        if (serverDetectedDisconnectFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            disconnectedEventReceived = serverDetectedDisconnectFuture.get();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    EXPECT_TRUE(disconnectedEventReceived)
        << "Test Failed: Server did not detect the disconnection packet within timeout.";

    context.stop();
    if (ioThread.joinable()) {
        ioThread.join();
    }
}
