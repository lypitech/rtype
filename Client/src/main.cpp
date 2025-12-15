#include "cli_parser.hpp"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(const int argc, const char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    // Logger::getInstance().addSink<logger::LogFileSink>("logs/client_latest.log");

    Logger::initialize("R-Type Client", argc, argv, logger::BuildInfo::fromCMake());
    cli_parser::Parser p(argc, argv);

    rteng::GameEngine eng(p.getValue("-h").as<std::string>(), p.getValue("-p").as<int>());
    eng.init(800, 600, "Test");
    eng.run();
    return 0;
}
