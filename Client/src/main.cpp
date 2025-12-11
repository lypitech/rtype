#include <asio.hpp>

#include "callbacks/callbacks.hpp"
#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "logger/Thread.h"
#include "packets/client/join.hpp"
#include "rtnt/Core/client.hpp"

int main(const int argc, const char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/client_latest.log");

    Logger::initialize("R-Type Client", argc, argv, logger::BuildInfo::fromCMake());

    cli_parser::Parser p(argc, argv);

    if (p.hasFlag("--help") || argc < 2) {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl
                  << "\t[options]: -h <host> (The ip address of the server)" << std::endl
                  << "\t[options]: -p <port> (A short designating the port to listen to)" << std::endl;
        return 0;
    }

    asio::io_context con;
    rtnt::core::Client client(con);

    client.onConnect(rt::callback::onConnect);
    client.onDisconnect(rt::callback::onDisconnect);
    client.onMessage(rt::callback::onMessage);

    client.connect("127.0.0.1", 4242);

    std::thread ioThread([&con]() {
        logger::setThreadLabel("IoThread");
        LOG_DEBUG("Running context");
        con.run();
    });

    if (ioThread.joinable()) {
        ioThread.join();
    }
    LOG_INFO("Shutting down client...");
    return 0;
}
