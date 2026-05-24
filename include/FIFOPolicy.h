#pragma once
#include "EvictionPolicy.h"
#include <queue>
#include <unordered_map>
#include <stdexcept>

template <typename K>
class FIFOPolicy : public virtual EvictionPolicy<K> {
private:
    std::queue<K> insertionOrder;
    std::unordered_map<K, size_t> removed;

public:
    FIFOPolicy() = default;
    FIFOPolicy(const FIFOPolicy& other) = default;
    FIFOPolicy(FIFOPolicy&& other) noexcept = default;
    FIFOPolicy& operator=(const FIFOPolicy& other) = default;
    FIFOPolicy& operator=(FIFOPolicy&& other) noexcept = default;
    ~FIFOPolicy() = default;

    void onAccess([[maybe_unused]] const K& key) override {}

    void onInsert(const K& key) override {
        insertionOrder.push(key);
    }

    void onRemove(const K& key) override {
        removed[key]++;
    }

    K evict() override {
        if (insertionOrder.empty())
            throw std::underflow_error("FIFOPolicy: cannot evict from empty cache");
        while (!insertionOrder.empty()) {
            K key = insertionOrder.front();
            insertionOrder.pop();
            auto it = removed.find(key);
            if (it != removed.end() && it->second > 0) {
                it->second--;
                if (it->second == 0)
                    removed.erase(it);
                continue;
            }
            this->evictionCount++;
            return key;
        }
        throw std::underflow_error("FIFOPolicy: no valid keys to evict");
    }

    std::string getName() const override { return "FIFO"; }

    void reset() override {
        while (!insertionOrder.empty())
            insertionOrder.pop();
        removed.clear();
        this->evictionCount = 0;
    }

    std::vector<K> getKeysOrdered() const override {
        std::vector<K> result;
        std::unordered_map<K, size_t> staleCounts = removed;
        std::queue<K> copy = insertionOrder;
        while (!copy.empty()) {
            K key = copy.front();
            copy.pop();
            auto it = staleCounts.find(key);
            if (it != staleCounts.end() && it->second > 0) {
                it->second--;
                if (it->second == 0)
                    staleCounts.erase(it);
            } else {
                result.push_back(key);
            }
        }
        return result;
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
