#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"

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

    LOG_INFO("hello from client");
    return 0;
}
