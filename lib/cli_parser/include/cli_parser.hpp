#pragma once

#include <string>
#include <unordered_map>

namespace cli_parser {

class Parser {
   public:
    explicit Parser(int argc, const char* argv[]);
    ~Parser() = default;
    [[nodiscard]] std::string getValue(const std::string& flag);
    [[nodiscard]] bool hasFlag(const std::string& flag) const;

   private:
    std::unordered_map<std::string, std::string> _flags;
};

}  // namespace cli_parser