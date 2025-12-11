#include <gtest/gtest.h>

#include <asio/io_context.hpp>
#include <future>
#include <thread>

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "rtnt/Core/client.hpp"
#include "rtnt/Core/server.hpp"

struct Example
{
    static constexpr rtnt::core::packet::Id kId = 130608;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    template <typename Archive>
    void serialize(Archive&)
    {
        // Not serializing anything, meaning the packet doesn't have a body (payload).
    }
};

TEST(rtnt, empty_packet)
{
    asio::io_context context;
    auto workGuard = asio::make_work_guard(context);

    LOG_DEBUG("Initializing server and client");
    rtnt::core::Server server(context, 4242);
    rtnt::core::Client client(context);

    std::promise<bool> promise;
    auto future = promise.get_future();

    LOG_DEBUG("Setting onMessage function of server");
    // server.onMessage([](const auto& session, auto& packet) {
    //     LOG_DEBUG("Server received a message!");
    // });

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

    client.onConnect([&]() {
        LOG_DEBUG("Handshake is done, client is connected!");
        promise.set_value(true);
    });

    client.onDisconnect([]() { LOG_DEBUG("Client disconnected"); });

    client.onMessage([](rtnt::core::Packet&) {});

    LOG_DEBUG("Will connect");
    client.connect("127.0.0.1", 4242);

    Example ex{};

    client.send(ex);

    const std::future_status status = future.wait_for(std::chrono::seconds(2));

    EXPECT_EQ(status, std::future_status::ready) << "Test timed out!";

    if (status == std::future_status::ready) {
        EXPECT_TRUE(future.get());
    }

    context.stop();
    if (ioThread.joinable()) {
        ioThread.join();
    }
}
