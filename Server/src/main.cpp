#include "rtnt.h"
#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"

int main(int argc, char* argv[])
{
    Logger::getInstance().addSink<logger::ConsoleSink>();
    Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log");

    Logger::initialize(
        "R-Type Server",
        argc, const_cast<const char**>(argv),
        logger::BuildInfo::fromCMake()
    );

    rtnt::hello();

    LOG_INFO("hello from server");

    return 0;
}
