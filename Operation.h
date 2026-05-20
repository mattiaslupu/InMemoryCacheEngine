
#pragma once
#include <string>
#include <iostream>

enum class OpType {
    PUT,
    GET,
    REMOVE
};

class Operation {
private:
    OpType type;
    std::string key;
    std::string value;

public:
    Operation();
    Operation(OpType type, const std::string& key, const std::string& value = "");
    Operation(const Operation& obj) = default;
    Operation(Operation&& obj) noexcept = default;
    Operation& operator=(const Operation& obj) = default;
    Operation& operator=(Operation&& obj) noexcept = default;
    ~Operation() = default;

    OpType getType() const;
    std::string getKey() const;
    std::string getValue() const;

    friend std::ostream& operator<<(std::ostream& os, const Operation& obj);
    friend std::istream& operator>>(std::istream& is, Operation& obj);
};