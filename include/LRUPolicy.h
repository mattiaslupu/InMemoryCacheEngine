#pragma once
#include "EvictionPolicy.h"
#include <list>
#include <unordered_map>
#include <stdexcept>

template <typename K>
class LRUPolicy : public virtual EvictionPolicy<K> {
protected:
    std::list<K> accessOrder;
    std::unordered_map<K, typename std::list<K>::iterator> keyToNode;

    void moveToFront(const K& key) {
        auto it = keyToNode[key];
        accessOrder.erase(it);
        accessOrder.push_front(key);
        keyToNode[key] = accessOrder.begin();
    }

public:
    LRUPolicy() = default;

    LRUPolicy(const LRUPolicy& obj) : EvictionPolicy<K>(obj), accessOrder{obj.accessOrder}, keyToNode{} {
        for (auto it = accessOrder.begin(); it != accessOrder.end(); ++it)
            keyToNode[*it] = it;
    }

    LRUPolicy& operator=(const LRUPolicy& obj) {
        if (this != &obj) {
            EvictionPolicy<K>::operator=(obj);
            accessOrder = obj.accessOrder;
            keyToNode.clear();
            for (auto it = accessOrder.begin(); it != accessOrder.end(); ++it)
                keyToNode[*it] = it;
        }
        return *this;
    }

    LRUPolicy(LRUPolicy&& other) noexcept = default;
    LRUPolicy& operator=(LRUPolicy&& other) noexcept = default;
    ~LRUPolicy() = default;

    void onAccess(const K& key) override {
        if (keyToNode.find(key) == keyToNode.end())
            throw std::invalid_argument("LRUPolicy: key not found");
        moveToFront(key);
    }

    void onInsert(const K& key) override {
        if (keyToNode.find(key) != keyToNode.end())
            throw std::invalid_argument("LRUPolicy: key already exists");
        accessOrder.push_front(key);
        keyToNode[key] = accessOrder.begin();
    }

    void onRemove(const K& key) override {
        if (keyToNode.find(key) == keyToNode.end())
            throw std::invalid_argument("LRUPolicy: key not found");
        auto it = keyToNode[key];
        accessOrder.erase(it);
        keyToNode.erase(key);
    }

    K evict() override {
        if (accessOrder.empty())
            throw std::underflow_error("LRUPolicy: cannot evict from empty cache");
        K element = accessOrder.back();
        accessOrder.pop_back();
        keyToNode.erase(element);
        this->evictionCount++;
        return element;
    }

    std::string getName() const override { return "LRU"; }

    void reset() override {
        accessOrder.clear();
        keyToNode.clear();
        this->evictionCount = 0;
    }

    template <typename KK>
    friend std::ostream& operator<<(std::ostream& os, const LRUPolicy<KK>& obj);
};

template <typename K>
std::ostream& operator<<(std::ostream& os, const LRUPolicy<K>& obj) {
    os << static_cast<const EvictionPolicy<K>&>(obj);
    os << "Access Order: ";
    for (const auto& key : obj.accessOrder)
        os << key << " ";
    os << "\n";
    return os;
}
