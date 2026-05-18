#include "Operation.h"
#include <stdexcept>

Operation::Operation()
    : type{OpType::GET}, key{""}, value{""} {}

Operation::Operation(OpType type, const std::string& key, const std::string& value)
    : type{type}, key{key}, value{value} {
    if (key.empty())
        throw std::invalid_argument("Operation: key cannot be empty");
}

Operation::Operation(const Operation& obj)
    : type{obj.type}, key{obj.key}, value{obj.value} {}

Operation::Operation(Operation&& obj) noexcept
    : type{obj.type},
      key{std::move(obj.key)},
      value{std::move(obj.value)} {}

Operation& Operation::operator=(const Operation& obj) {
    if (this != &obj) {
        type = obj.type;
        key = obj.key;
        value = obj.value;
    }
    return *this;
}

Operation& Operation::operator=(Operation&& obj) noexcept {
    if (this != &obj) {
        type = obj.type;
        key = std::move(obj.key);
        value = std::move(obj.value);
    }
    return *this;
}

Operation::~Operation() {}

OpType Operation::getType() const {
    return type;
}

std::string Operation::getKey() const {
    return key;
}

std::string Operation::getValue() const {
    return value;
}

std::ostream& operator<<(std::ostream& os, const Operation& obj) {
    switch (obj.type) {
        case OpType::PUT:    os << "PUT";    break;
        case OpType::GET:    os << "GET";    break;
        case OpType::REMOVE: os << "REMOVE"; break;
    }
    os << " " << obj.key;
    if (obj.type == OpType::PUT)
        os << " " << obj.value;
    os << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Operation& obj) {
    std::string typeStr;
    if (!(is >> typeStr))
        throw std::invalid_argument("Operation: invalid input");
    if (typeStr == "PUT")         obj.type = OpType::PUT;
    else if (typeStr == "GET")    obj.type = OpType::GET;
    else if (typeStr == "REMOVE") obj.type = OpType::REMOVE;
    else throw std::invalid_argument("Operation: unknown type: " + typeStr);
    if (!(is >> obj.key))
        throw std::invalid_argument("Operation: missing key");
    if (obj.type == OpType::PUT)
        if (!(is >> obj.value))
            throw std::invalid_argument("Operation: missing value for PUT");
    return is;
}