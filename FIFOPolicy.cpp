#include "FIFOPolicy.h"

std::string FIFOPolicy::getName() const {
    return "FIFO";
}

void FIFOPolicy::onAccess(const std::string &key) {

}

void FIFOPolicy::onInsert(const std::string& key) {
    if (removed.count(key))
        removed.erase(key);
    insertionOrder.push(key);
}

void FIFOPolicy::onRemove(const std::string &key) {
    removed.insert(key);
}

std::string FIFOPolicy::evict() {
    if (insertionOrder.empty())
        throw std::underflow_error("FIFOPolicy: cannot evict from empty cache");
    while (!insertionOrder.empty() && removed.count(insertionOrder.front())) {
        removed.erase(insertionOrder.front());
        insertionOrder.pop();
    }
    if (insertionOrder.empty())
        throw std::underflow_error("FIFOPolicy: no valid keys to evict");
    std::string key = insertionOrder.front();
    insertionOrder.pop();
    evictionCount++;
    return key;
}

void FIFOPolicy::reset() {
    while (!insertionOrder.empty())
        insertionOrder.pop();
    removed.clear();
    evictionCount = 0;
}

std::ostream& operator<<(std::ostream& os, const FIFOPolicy& obj) {
    os << static_cast<const EvictionPolicy&>(obj);
    os << "Insertion Order size: " << obj.insertionOrder.size() << "\n";
    return os;
}
