#pragma once
#include "EvictionPolicy.h"
#include <queue>
#include <unordered_set>
#include <stdexcept>

template <typename K>
class FIFOPolicy : public virtual EvictionPolicy<K> {
private:
    std::queue<K> insertionOrder;
    std::unordered_set<K> removed;

public:
    FIFOPolicy() = default;
    FIFOPolicy(const FIFOPolicy& other) = default;
    FIFOPolicy(FIFOPolicy&& other) noexcept = default;
    FIFOPolicy& operator=(const FIFOPolicy& other) = default;
    FIFOPolicy& operator=(FIFOPolicy&& other) noexcept = default;
    ~FIFOPolicy() = default;

    void onAccess([[maybe_unused]] const K& key) override {}

    void onInsert(const K& key) override {
        if (removed.count(key))
            removed.erase(key);
        insertionOrder.push(key);
    }

    void onRemove(const K& key) override {
        removed.insert(key);
    }

    K evict() override {
        if (insertionOrder.empty())
            throw std::underflow_error("FIFOPolicy: cannot evict from empty cache");
        while (!insertionOrder.empty() && removed.count(insertionOrder.front())) {
            removed.erase(insertionOrder.front());
            insertionOrder.pop();
        }
        if (insertionOrder.empty())
            throw std::underflow_error("FIFOPolicy: no valid keys to evict");
        K key = insertionOrder.front();
        insertionOrder.pop();
        this->evictionCount++;
        return key;
    }

    std::string getName() const override { return "FIFO"; }

    void reset() override {
        while (!insertionOrder.empty())
            insertionOrder.pop();
        removed.clear();
        this->evictionCount = 0;
    }

    template <typename KK>
    friend std::ostream& operator<<(std::ostream& os, const FIFOPolicy<KK>& obj);
};

template <typename K>
std::ostream& operator<<(std::ostream& os, const FIFOPolicy<K>& obj) {
    os << static_cast<const EvictionPolicy<K>&>(obj);
    os << "Insertion Order size: " << obj.insertionOrder.size() << "\n";
    return os;
}
