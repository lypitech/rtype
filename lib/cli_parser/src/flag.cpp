#include "flag.hpp"

#include <locale>
#include <sstream>
#include <utility>

namespace cli_parser {

Flag::Flag(std::string flag, std::string value)
    : _flag(std::move(flag)), _value(std::move(value)) {
    detectType();
}

void Flag::detectType() {
    if (_value.empty()) {
        _type = std::type_index(typeid(std::string));
        return;
    }
    if (_value == "true" || _value == "false") {
        _type = std::type_index(typeid(bool));
    } else if (_value.find('.') != std::string::npos) {
        std::istringstream iss(_value);
        double d;
        _type = std::type_index(iss >> d ? typeid(double) : typeid(float));
    } else {
        std::istringstream iss(_value);
        int i;
        _type = std::type_index(iss >> i ? typeid(int) : typeid(std::string));
    }
}

bool Flag::operator!=(const std::string& other) const { return _flag != other; }

bool Flag::operator==(const std::string& other) const { return _flag == other; }

}  // namespace cli_parser
