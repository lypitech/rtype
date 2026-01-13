#include "app.hpp"
#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(int argc,
         const char** argv)
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    // Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log");

    Logger::initialize("R-Type Server", argc, argv, logger::BuildInfo::fromCMake());
    cli_parser::Parser p(argc, argv);

    if (!p.hasFlag("-p")) {
        LOG_FATAL("No port specified, use \"-p {port}\".");
        return 84;
    }
    if (!p.hasFlag("--config")) {
        LOG_FATAL("No config specified, use \"--config {filepath}\".");
        return 84;
    }

    server::App server(p.getValue("-p").as<int>(), p.getValue("--config").as<std::string>());
    server.start();
}
