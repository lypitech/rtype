#pragma once

#include <exception>
#include <string>

namespace rtecs::exceptions {

class BaseException : public std::exception
{
private:
    const std::string &_kMessage;
protected:
    /**
     * Generate a new error from the ECS.
     *
     * @param kMessage The log message of the exception.
     */
    explicit BaseException(const std::string &kMessage);

    /**
     * @return The message of the exception.
     */
    [[nodiscard]]
    const char* what() const noexcept override;
};

}
