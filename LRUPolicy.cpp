
#include "LRUPolicy.h"

LRUPolicy::LRUPolicy() : EvictionPolicy() {}

LRUPolicy::LRUPolicy(const LRUPolicy &obj) : EvictionPolicy(obj){
}

LRUPolicy &LRUPolicy::operator=(const LRUPolicy &obj) {
    if (this!= &obj) {
        EvictionPolicy::operator=(obj);
    }
    return *this;
}

LRUPolicy::~LRUPolicy() {

}

void LRUPolicy::moveToFront(const std::string &key) {
    auto it = keyToNode[key];
    accessOrder.erase(it);
    accessOrder.push_front(key);
    keyToNode[key] = accessOrder.begin();
}

std::string LRUPolicy::evict() {
    auto element = accessOrder.back();
    accessOrder.pop_back();
    return element;
}

std::string LRUPolicy::getName() const {
    return "LRU";
}

void LRUPolicy::onAccess(const std::string &key) {
    moveToFront(key);
}

void LRUPolicy::onInsert(const std::string &key) {
    accessOrder.push_front(key);
    keyToNode[key] = accessOrder.begin();
}

void LRUPolicy::onRemove(const std::string& key) {
    auto it = keyToNode[key];
    accessOrder.erase(it);
    keyToNode.erase(key);
}

void LRUPolicy::reset() {
    accessOrder.clear();
    keyToNode.clear();
    evictionCount = 0;
}

