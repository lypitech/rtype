#pragma once

#include <gtest/gtest.h>

#include <asio/io_context.hpp>
#include <future>
#include <thread>

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "rtnt/core/client.hpp"
#include "rtnt/core/server.hpp"

class NetworkTest : public testing::Test
{
protected:
    asio::io_context context;
    asio::executor_work_guard<asio::io_context::executor_type> workGuard;

    std::unique_ptr<rtnt::core::Server> server;
    std::unique_ptr<rtnt::core::Client> client;

    std::thread ioThread;

    NetworkTest()
        : workGuard(asio::make_work_guard(context))
    {
    }

    void SetUp() override
    {
        server = std::make_unique<rtnt::core::Server>(context, 4242);
        client = std::make_unique<rtnt::core::Client>(context);

        server->start();

        ioThread = std::thread([this]() {
            logger::setThreadLabel("IoThread");
            context.run();
        });
    }

    void TearDown() override
    {
        context.stop();

        if (ioThread.joinable()) {
            ioThread.join();
        }

        client.reset();
        server.reset();

        context.restart();
    }

    template <typename Func>
    bool waitFor(Func condition, std::chrono::milliseconds timeout = std::chrono::seconds(2))
    {
        const auto start = std::chrono::steady_clock::now();

        while (std::chrono::steady_clock::now() - start < timeout) {
            if (server) {
                server->update();
            }
            if (client) {
                client->update();
            }

            if (condition()) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
        }
        return false;
    }
};
