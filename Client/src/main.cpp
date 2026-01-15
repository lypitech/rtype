#include "app.hpp"
#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(const int argc,
         const char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    // Logger::getInstance().addSink<logger::LogFileSink>("logs/client_latest.log");

    Logger::initialize("R-Type Client", argc, argv, logger::BuildInfo::fromCMake());
    cli_parser::Parser p(argc, argv);

    if (!p.hasFlag("-h")) {
        LOG_FATAL("No host specified, use \"-h <host>\"");
    }
    if (!p.hasFlag("-p")) {
        LOG_FATAL("No port specified, use \"-p <port>\"");
    }
    client::App client(
        p.getValue("-h").as<std::string>(), static_cast<short>(p.getValue("-p").as<int>()));
    client.run();
    return 0;
}
