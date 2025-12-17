#pragma once

#include <string>
#include <vector>

#include "flag.hpp"

namespace cli_parser {

/**
 * @class   Parser
 * @brief   A simple command line interface parser.
 *
 * The parser permits to access to the parameter given to the binary when
 * starting.
 */
class Parser
{
public:
    /**
     * @brief Use the basic argument of the function main.
     *
     * @param argc The number of element in argv.
     * @param argv The arguments given to the program.
     */
    explicit Parser(int argc,
                    const char* argv[]);
    ~Parser() = default;

    /**
     *
     * @brief Get the value of a certain flag.
     *
     * @param flag The flag you are searching for.
     * @return A Flag instance, empty if the flag couldn't be found.
     */
    [[nodiscard]]
    Flag getValue(const std::string& flag);

    /**
     *
     * @brief Checks if a certain flag is present
     *
     * @param flag The flag to check the presence for.
     * @return A boolean corresponding to the presence of the flag.
     */
    [[nodiscard]]
    bool hasFlag(const std::string& flag) const;

private:
    std::vector<Flag> _flags{};
};

}  // namespace cli_parser
