#include "Operation.h"
#include <stdexcept>

Operation::Operation()
    : type{OpType::GET}, key{""}, value{""} {}

Operation::Operation(OpType type, const std::string& key, const std::string& value)
    : type{type}, key{key}, value{value} {
    if (key.empty())
        throw std::invalid_argument("Operation: key cannot be empty");
}


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