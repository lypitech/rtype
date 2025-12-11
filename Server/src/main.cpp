#include <asio.hpp>

#include "callbacks/callbacks.hpp"
#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "logger/Thread.h"
#include "rtnt/Core/Server.hpp"

int main(const int argc, const char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log");

    Logger::initialize("R-Type Server", argc, (argv), logger::BuildInfo::fromCMake());

    cli_parser::Parser p(argc, argv);

    if (p.hasFlag("--help") || argc < 2) {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl
                  << "\t[options]: -p <port> (A short designating the port to listen to)" << std::endl
                  << "\t[options]: -m <map> (The path to the map infos in json format)" << std::endl;
        return 0;
    }

    asio::io_context con;
    rtnt::core::Server srv(con, p.getValue("-p").as<int>());

    srv.onConnect(rt::callback::onConnect);
    srv.onDisconnect(rt::callback::onDisconnect);
    srv.onMessage(rt::callback::onMessage);
    srv.start();

    std::thread ioThread([&con]() {
        logger::setThreadLabel("IoThread");
        LOG_DEBUG("Running context");
        con.run();
    });
    if (ioThread.joinable()) {
        ioThread.join();
    }
    LOG_INFO("Shutting down server...");
    return 0;
}
