#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <typeindex>

#define IF_T_IS_TYPE(t) if constexpr (std::is_same_v<T, t>)

namespace cli_parser {

/**
 * @class Flag
 *
 * @brief An encapsulation of the concept of flag.
 *
 * This class is mainly there for the value to be cast to the wanted type.
 */
class Flag
{
public:
    /**
     * @brief Creates an instance of the class Flag and recover its type.
     * @warning This class is not supposed to be instantiated by the user.
     *
     * @param flag The "key" of the flag to create
     * @param value The "value" of the flag to create
     */
    explicit Flag(std::string flag,
                  std::string value);
    ~Flag() = default;

    /**
     * @brief Converts the value of the flag to required type.
     * @tparam T The type to convert the value to.
     * @return The value converted to the requested type, an empty instance of
     * the requested type if incompatible type.
     */
    template <typename T>
    T as() const
    {
        IF_T_IS_TYPE(bool) { return _value == "true" || _value == "1"; }
        IF_T_IS_TYPE(std::string) { return _value; }
        IF_T_IS_TYPE(int) { return std::stoi(_value); }
        IF_T_IS_TYPE(double)
        {
            if (_type != typeid(int) && _type != typeid(double)) {
                return T();
            }
            return std::stod(_value);
        }
        return T();
    }

    /**
     * @brief Compares the "key" of the flag to the other operand.
     * @param other A string corresponding to the researched key.
     * @return A boolean corresponding to the equality of the operands.
     */
    bool operator==(const std::string& other) const;
    /**
     * @brief Compares the "key" of the flag to the other operand.
     * @param other A string corresponding to the researched key.
     * @return A boolean corresponding to the inequality of the operands.
     */
    bool operator!=(const std::string& other) const;

private:
    std::string _flag;
    std::string _value;
    std::type_index _type = std::type_index(typeid(std::string));

    /**
     * @brief Detects the type of this flag and stores it in member @code
     * _type@endcode
     */
    void detectType();
};

}  // namespace cli_parser
