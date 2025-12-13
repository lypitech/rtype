#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(const int argc, const char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/client_latest.log");

    Logger::initialize("R-Type Client", argc, argv, logger::BuildInfo::fromCMake());

    rteng::GameEngine eng("127.0.0.1", 4242);
    eng.init(1920, 1080, "Test");
    eng.run();
    return 0;
}
