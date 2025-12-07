#include <future>
#include <thread>
#include <gtest/gtest.h>
#include <asio/io_context.hpp>

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "rtnt/Core/Client.hpp"
#include "rtnt/Core/Server.hpp"

TEST(echo, basic)
{
    asio::io_context context;
    auto workGuard = asio::make_work_guard(context);

    LOG_DEBUG("Initializing server and client");
    rtnt::core::Server server(context, 4242);
    rtnt::core::Client client(context);

    std::promise<std::string> promise;
    auto future = promise.get_future();

    LOG_DEBUG("Setting onMessage function of server");
    server.onMessage([](const auto& session, auto& packet) {
        LOG_DEBUG("Server received a message!");
        session->send(packet);
    });

    server.onDisconnect([](const auto& session) {
        LOG_DEBUG("Session {} disconnected", session.get()->getEndpoint().address().to_string());
    });

    server.onConnect([](const auto& session) {
        LOG_DEBUG("Session {} connected", session.get()->getEndpoint().address().to_string());
    });

    LOG_DEBUG("Starting server");
    server.start();

    LOG_DEBUG("Will run context in another thread");
    std::thread ioThread([&context]() {
        logger::setThreadLabel("IoThread");
        LOG_DEBUG("Running context");
        context.run();
    });

    LOG_DEBUG("Now waiting 400ms");
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    client.onDisconnect([]() {
        LOG_DEBUG("Client disconnected");
    });

    client.onMessage([&](auto& packet) {
        std::string response;
        try {
            packet >> response;
            promise.set_value(response);
            LOG_DEBUG("Message!! {}", response);
        } catch (const std::exception& e) {
            LOG_ERR("{}", e.what());
        }
    });

    LOG_DEBUG("Will connect");
    client.connect("127.0.0.1", 4242);

    rtnt::core::Packet p(10, rtnt::core::packet::Flag::kUnreliable);
    std::string message = "Hello GoogleTest";
    p << message;

    client.send(p);

    std::future_status status = future.wait_for(std::chrono::seconds(2));

    EXPECT_EQ(status, std::future_status::ready) << "Test timed out!";

    if (status == std::future_status::ready) {
        EXPECT_EQ(future.get(), "Hello GoogleTest");
    }

    context.stop();
    if (ioThread.joinable()) {
        ioThread.join();
    }
}
