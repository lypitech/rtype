#include "app.hpp"
#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(int argc,
         const char** argv)
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log");

    Logger::initialize("R-Type Server", argc, argv, logger::BuildInfo::fromCMake());
    cli_parser::Parser p(argc, argv);

    if (!p.hasFlag("-p")) {
        LOG_FATAL("No port specified, use \"-p {port}\".");
    }
    if (p.hasFlag("--graphical")) {
        LOG_INFO("Running server with debug window. (unimplemented)");
    }

    server::App server(p.getValue("-p").as<unsigned short>());
    server.start();
}
