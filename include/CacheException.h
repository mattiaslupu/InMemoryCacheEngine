#pragma once
#include <stdexcept>
#include <string>

class CacheException : public std::exception {
private:
    std::string message;
public:
    explicit CacheException(const std::string& msg) : message{msg} {}
    const char* what() const noexcept override { return message.c_str(); }
};

class KeyNotFoundException : public CacheException {
public:
    explicit KeyNotFoundException(const std::string& key)
        : CacheException("Key not found: " + key) {}
};

class InvalidCapacityException : public CacheException {
public:
    explicit InvalidCapacityException()
        : CacheException("Capacity must be greater than 0") {}
};

class PolicyPtrException : public CacheException {
public:
    explicit PolicyPtrException() : CacheException("Cache: policy cannot be null"){}
};