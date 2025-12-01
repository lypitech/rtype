#pragma once

#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "logger/Sinks/LogFileSink.h"

class LogEnvironment final : public testing::Environment
{
public:
    LogEnvironment(const int argc, char** argv)
        : _argc(argc), _argv(argv) {}

    void SetUp() override
    {
        const std::string projectName{ "rtecs (tests)" };

        Logger::getInstance().addSink<logger::ConsoleSink>();
        Logger::getInstance().addSink<logger::LogFileSink>("logs/latest.log");
        Logger::getInstance().addSink<logger::LogFileSink>(std::format(
            "logs/{}",
            Logger::generateLogFileName(projectName, ".log")
        ));

        Logger::initialize(
            projectName,
            _argc, const_cast<const char **>(_argv),
            logger::BuildInfo::fromCMake()
        );
    }

    void TearDown() override
    {
        Logger::getInstance().shutdown();
    }

private:
    int _argc;
    char** _argv;
};

class LoggingListener : public testing::EmptyTestEventListener
{

    void OnTestStart(const testing::TestInfo& testInfo) override
    {
        LOG_INFO("[{}] Starting test.", testInfo.name());
    }
    void OnTestEnd(const testing::TestInfo& testInfo) override
    {
        LOG_INFO("[{}] Test ended: {}.", testInfo.name(), testInfo.result()->Passed() ? "OK" : "KO");
    }

};
