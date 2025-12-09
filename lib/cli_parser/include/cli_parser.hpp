#pragma once

#include <string>
#include <vector>

#include "flag.hpp"

namespace cli_parser {

class Parser {
public:
    explicit Parser(int argc, const char* argv[]);
    ~Parser() = default;

    [[nodiscard]]
    Flag getValue(const std::string& flag);
    [[nodiscard]]
    bool hasFlag(const std::string& flag) const;

private:
    std::vector<Flag> _flags{};
};

}  // namespace cli_parser
