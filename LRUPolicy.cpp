#include "LRUPolicy.h"
#include <stdexcept>


LRUPolicy::LRUPolicy(const LRUPolicy& obj) : EvictionPolicy(obj),
    accessOrder{obj.accessOrder},
    keyToNode{} {
    for (auto it = accessOrder.begin(); it != accessOrder.end(); it++)
        keyToNode[*it] = it;
}

LRUPolicy& LRUPolicy::operator=(const LRUPolicy& obj) {
    if (this != &obj) {
        EvictionPolicy::operator=(obj);
        accessOrder = obj.accessOrder;
        keyToNode.clear();
        for (auto it = accessOrder.begin(); it != accessOrder.end(); it++)
            keyToNode[*it] = it;
    }
    return *this;
}


void LRUPolicy::moveToFront(const std::string& key) {
    auto it = keyToNode[key];
    accessOrder.erase(it);
    accessOrder.push_front(key);
    keyToNode[key] = accessOrder.begin();
}

std::string LRUPolicy::evict() {
    if (accessOrder.empty())
        throw std::underflow_error("LRUPolicy: cannot evict from empty cache");
    std::string element = accessOrder.back();
    accessOrder.pop_back();
    keyToNode.erase(element);
    evictionCount++;
    return element;
}

std::string LRUPolicy::getName() const {
    return "LRU";
}

void LRUPolicy::onAccess(const std::string& key) {
    if (keyToNode.find(key) == keyToNode.end())
        throw std::invalid_argument("LRUPolicy: key not found: " + key);
    moveToFront(key);
}

void LRUPolicy::onInsert(const std::string& key) {
    if (keyToNode.find(key) != keyToNode.end())
        throw std::invalid_argument("LRUPolicy: key already exists: " + key);
    accessOrder.push_front(key);
    keyToNode[key] = accessOrder.begin();
}

void LRUPolicy::onRemove(const std::string& key) {
    if (keyToNode.find(key) == keyToNode.end())
        throw std::invalid_argument("LRUPolicy: key not found: " + key);
    auto it = keyToNode[key];
    accessOrder.erase(it);
    keyToNode.erase(key);
}

void LRUPolicy::reset() {
    accessOrder.clear();
    keyToNode.clear();
    evictionCount = 0;
}

std::ostream& operator<<(std::ostream& os, const LRUPolicy& obj) {
    os << static_cast<const EvictionPolicy&>(obj);
    os << "Access Order: ";
    for (const auto& key : obj.accessOrder)
        os << key << " ";
    os << "\n";
    return os;
}