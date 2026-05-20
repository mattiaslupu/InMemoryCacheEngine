#include "LRUWithTTL.h"


LRUWithTTL::LRUWithTTL(time_t defaultTTL)  : EvictionPolicy(),LRUPolicy(), TTLPolicy(defaultTTL) {

}

LRUWithTTL::LRUWithTTL(const LRUWithTTL &obj) : EvictionPolicy(obj), LRUPolicy(obj), TTLPolicy(obj) {

}

LRUWithTTL &LRUWithTTL::operator=(const LRUWithTTL &obj) {
    if (this != &obj) {
        EvictionPolicy::operator=(obj);
        LRUPolicy::operator=(obj);
        TTLPolicy::operator=(obj);
    }
    return *this;
}


std::string LRUWithTTL::getName() const {
    return "LRU + TTL";
}


std::string LRUWithTTL::evict() {
    if (accessOrder.empty())
        throw std::underflow_error("LRUWithTTL: cannot evict from empty cache");

    for (const auto& pair : expirations) {
        if (isExpired(pair.first)) {
            std::string key = pair.first;
            expirations.erase(key);
            auto it = keyToNode[key];
            accessOrder.erase(it);
            keyToNode.erase(key);
            evictionCount++;
            return key;
        }
    }

    std::string element = accessOrder.back();
    accessOrder.pop_back();
    keyToNode.erase(element);
    expirations.erase(element);
    evictionCount++;
    return element;
}

void LRUWithTTL::onAccess(const std::string &key) {
    LRUPolicy::onAccess(key);
}

void LRUWithTTL::reset() {
    LRUPolicy::reset();
    TTLPolicy::reset();
}

void LRUWithTTL::onInsert(const std::string &key) {
    LRUPolicy::onInsert(key);
    TTLPolicy::onInsert(key);
}

void LRUWithTTL::onRemove(const std::string &key) {
    LRUPolicy::onRemove(key);
    TTLPolicy::onRemove(key);
}

std::ostream& operator<<(std::ostream& os, const LRUWithTTL& obj) {
    os << static_cast<const EvictionPolicy&>(obj);
    os << "Access Order: ";
    for (const auto& key : obj.accessOrder)
        os << key << " ";
    os << "\n";
    os << "Default TTL: " << obj.defaultTTL << " seconds\n";
    os << "Active expirations: " << obj.expirations.size() << "\n";
    return os;
}
