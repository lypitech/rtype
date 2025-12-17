#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(int argc,
         const char** argv)
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log");

    Logger::initialize(
        "R-Type Server", argc, const_cast<const char**>(argv), logger::BuildInfo::fromCMake());
    cli_parser::Parser p(argc, argv);

    rteng::GameEngine eng(p.getValue("-p").as<int>());
    eng.init();
    eng.run();
    LOG_INFO("Shutting down server.");

    return 0;
}
