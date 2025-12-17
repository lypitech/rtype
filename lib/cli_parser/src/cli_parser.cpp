#include "cli_parser.hpp"

namespace cli_parser {

Parser::Parser(const int argc,
               const char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg[0] != '-') {
            continue;
        }
        i++;
        if (i >= argc) {
            _flags.emplace_back(arg, "");
            break;
        }
        if (argv[i][0] == '-') {
            i--;
            _flags.emplace_back(arg, "");
        } else {
            _flags.emplace_back(arg, std::string(argv[i]));
        }
    }
}

Flag Parser::getValue(const std::string& flag)
{
    for (auto& f : _flags) {
        if (f == flag) {
            return f;
        }
    }
    return Flag{"", ""};
}

bool Parser::hasFlag(const std::string& flag) const
{
    for (auto& f : _flags) {
        if (f == flag) {
            return true;
        }
    }
    return false;
}

}  // namespace cli_parser
