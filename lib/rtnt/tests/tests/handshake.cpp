#include <gtest/gtest.h>

#include <asio/io_context.hpp>
#include <future>
#include <thread>

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "rtnt/core/client.hpp"
#include "rtnt/core/server.hpp"

TEST(rtnt, handshake)
{
    asio::io_context context;
    auto workGuard = asio::make_work_guard(context);

    LOG_DEBUG("Initializing server and client");
    rtnt::core::Server server(context, 4242);
    rtnt::core::Client client(context);

    std::promise<bool> clientConnectedPromise;
    std::future<bool> clientConnectedFuture = clientConnectedPromise.get_future();

    server.onConnect([](const std::shared_ptr<rtnt::core::Session>& session) {
        LOG_DEBUG("Server: New session ({}) connected from {}", session->getId(), session->getEndpoint().port());
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

    context.stop();
    if (ioThread.joinable()) {
        ioThread.join();
    }
}
