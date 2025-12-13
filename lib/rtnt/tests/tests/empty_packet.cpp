#include <gtest/gtest.h>

#include <asio/io_context.hpp>
#include <future>
#include <thread>

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "rtnt/core/client.hpp"
#include "rtnt/core/server.hpp"

struct Example
{
    static constexpr rtnt::core::packet::Id   kId   = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    template <typename Archive>
    void serialize(Archive&)
    {
        // Not serializing anything, meaning the packet doesn't have a body (payload).
    }

    static void onReceive(const std::shared_ptr<rtnt::core::Session>& session, const Example& pkt)
    {
        LOG_INFO("Example packet received!");
    }
};

TEST(rtnt, empty_packet)
{
    asio::io_context context;
    auto workGuard = asio::make_work_guard(context);

    LOG_DEBUG("Initializing server and client");
    rtnt::core::Server server(context, 4242);
    rtnt::core::Client client(context);

    std::promise<bool> clientConnectedPromise;
    std::future<bool> clientConnectedFuture = clientConnectedPromise.get_future();

    std::promise<size_t> packetSizePromise;
    std::future<size_t> packetSizeFuture = packetSizePromise.get_future();

    server.getPacketDispatcher().bind<Example>();

    server.onConnect([](const std::shared_ptr<rtnt::core::Session>& session) {
        LOG_DEBUG("Server: New session ({}) connected from {}", session->getId(), session->getEndpoint().port());
    });

    server.onDisconnect([&](const std::shared_ptr<rtnt::core::Session>& session) {
        LOG_INFO("Server: Session disconnected (Endpoint: {})", session->getEndpoint().port());
    });

    server.onMessage([&](const std::shared_ptr<rtnt::core::Session>& /*session*/, const rtnt::core::Packet& packet) {
        if (packet.getId() == Example::kId) {
            packetSizePromise.set_value(packet.getPayload().size());
        }
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

    constexpr Example ex{};
    client.send(ex);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto startTime = std::chrono::steady_clock::now();
    uint32_t packetSize = 0;
    bool packetReceived = false;

    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(3)) {
        server.update();
        client.update();

        if (packetSizeFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            packetSize = packetSizeFuture.get();
            packetReceived = true;
            client.disconnect();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    ASSERT_TRUE(packetReceived) << "Test Failed: Server did not receive packet within timeout.";

    EXPECT_EQ(packetSize, 0)
        << std::format("Test Failed: Server did not received the correct packet size (got {}, expected 0).", packetSize);

    context.stop();
    if (ioThread.joinable()) {
        ioThread.join();
    }
}
