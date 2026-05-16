#include "TTLPolicy.h"
#include <stdexcept>

TTLPolicy::TTLPolicy() : EvictionPolicy(), defaultTTL{60} {}

TTLPolicy::TTLPolicy(time_t defaultTTL) : EvictionPolicy(), defaultTTL{defaultTTL} {
    if (defaultTTL < 0)
        throw std::invalid_argument("TTLPolicy: TTL cannot be negative");
}

TTLPolicy::TTLPolicy(const TTLPolicy& obj)
    : EvictionPolicy(obj),
      expirations{obj.expirations},
      defaultTTL{obj.defaultTTL} {}

TTLPolicy& TTLPolicy::operator=(const TTLPolicy& obj) {
    if (this != &obj) {
        EvictionPolicy::operator=(obj);
        expirations = obj.expirations;
        defaultTTL = obj.defaultTTL;
    }
    return *this;
}

TTLPolicy::~TTLPolicy() {}

std::string TTLPolicy::getName() const {
    return "TTL";
}

bool TTLPolicy::isExpired(const std::string& key) const {
    if (expirations.find(key) == expirations.end())
        return false;
    return std::time(nullptr) > expirations.at(key);
}



void TTLPolicy::onAccess(const std::string& key) {}

void TTLPolicy::onInsert(const std::string& key) {
    if (expirations.find(key) != expirations.end())
        throw std::invalid_argument("TTLPolicy: key already exists: " + key);
    expirations[key] = std::time(nullptr) + defaultTTL;
}

void TTLPolicy::onRemove(const std::string& key) {
    if (expirations.find(key) == expirations.end())
        throw std::invalid_argument("TTLPolicy: key not found: " + key);
    expirations.erase(key);
}

std::string TTLPolicy::evict() {
    if (expirations.empty())
        throw std::underflow_error("TTLPolicy: cannot evict from empty cache");

    for (const auto& pair : expirations) {
        if (isExpired(pair.first)) {
            std::string key = pair.first;
            expirations.erase(key);
            evictionCount++;
            return key;
        }
    }

    std::string key = expirations.begin()->first;
    expirations.erase(key);
    evictionCount++;
    return key;
}

void TTLPolicy::reset() {
    expirations.clear();
    evictionCount = 0;
}

void TTLPolicy::setDefaultTTL(time_t seconds) {
    if (seconds < 0)
        throw std::invalid_argument("TTLPolicy: TTL cannot be negative");
    defaultTTL = seconds;
}

time_t TTLPolicy::getDefaultTTL() const {
    return defaultTTL;
}

std::ostream& operator<<(std::ostream& os, const TTLPolicy& obj) {
    os << static_cast<const EvictionPolicy&>(obj);
    os << "Default TTL: " << obj.defaultTTL << " seconds\n";
    os << "Active keys: " << obj.expirations.size() << "\n";
    return os;
}