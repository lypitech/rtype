#include "cli_parser.hpp"

#include <iostream>

namespace cli_parser {

Parser::Parser(const int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg[0] != '-') {
            continue;
        }
        i++;
        if (i >= argc) {
            _flags.emplace(arg, "");
            break;
        }
        if (argv[i][0] == '-') {
            i--;
            _flags.emplace(arg, "");
        } else {
            _flags.emplace(arg, std::string(argv[i]));
        }
    }
}

std::string Parser::getValue(const std::string& flag) {
    if (_flags.contains(flag)) {
        return _flags.at(flag);
    }
    return "";
}

bool Parser::hasFlag(const std::string& flag) const {
    return _flags.contains(flag);
}

}  // namespace cli_parser
