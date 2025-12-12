#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(int argc, char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log");

    Logger::initialize("R-Type Server", argc, const_cast<const char**>(argv), logger::BuildInfo::fromCMake());

    rteng::GameEngine eng(4243);
    eng.init();
    eng.run();
    LOG_INFO("Shutting down server.");

    return 0;
}
