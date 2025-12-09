#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <typeindex>

#define IF_T_IS_TYPE(t) if constexpr (std::is_same_v<T, t>)

namespace cli_parser {

class Flag {
public:
    explicit Flag(std::string flag, std::string value);
    ~Flag() = default;


    template <typename T>
    T as() const {
        IF_T_IS_TYPE(bool) { return _value == "true" || _value == "1"; }
        IF_T_IS_TYPE(std::string) { return _value; }
        IF_T_IS_TYPE(int) { return std::stoi(_value); }
        IF_T_IS_TYPE(double) {
            if (_type != typeid(int) && _type != typeid(double)) {
                return T();
            }
            return std::stod(_value);
        }
        return T();
    }

    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

private:
    void detectType();
    std::string _flag;
    std::string _value;
    std::type_index _type = std::type_index(typeid(std::string));
};

}  // namespace cli_parser
