#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"
#include "rteng.hpp"

int main(const int argc, const char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/client_latest.log");

    Logger::initialize("R-Type Client", argc, const_cast<const char**>(argv), logger::BuildInfo::fromCMake());

    LOG_INFO("hello from client");
    rteng::GameEngine eng("127.0.0.1", 4242);
    eng.init(1920, 1080, "Test");
    eng.run();
    LOG_INFO("Shutting down client...");
    return 0;
}
