#pragma once
#include "LRUPolicy.h"
#include "TTLPolicy.h"
#include <stdexcept>

template <typename K>
class LRUWithTTL : public LRUPolicy<K>, public TTLPolicy<K> {
public:
    LRUWithTTL() = default;

    explicit LRUWithTTL(time_t defaultTTL)
        : EvictionPolicy<K>(), LRUPolicy<K>(), TTLPolicy<K>(defaultTTL) {}

    LRUWithTTL(const LRUWithTTL& other)
        : EvictionPolicy<K>(other), LRUPolicy<K>(other), TTLPolicy<K>(other) {}

    LRUWithTTL(LRUWithTTL&& other) noexcept = default;

    LRUWithTTL& operator=(const LRUWithTTL& other) {
        if (this != &other) {
            EvictionPolicy<K>::operator=(other);
            LRUPolicy<K>::operator=(other);
            TTLPolicy<K>::operator=(other);
        }
        return *this;
    }

    LRUWithTTL& operator=(LRUWithTTL&& other) noexcept = default;
    ~LRUWithTTL() = default;

    void onAccess(const K& key) override {
        LRUPolicy<K>::onAccess(key);
    }

    void onInsert(const K& key) override {
        LRUPolicy<K>::onInsert(key);
        TTLPolicy<K>::onInsert(key);
    }

    void onRemove(const K& key) override {
        LRUPolicy<K>::onRemove(key);
        TTLPolicy<K>::onRemove(key);
    }

    K evict() override {
        if (this->accessOrder.empty())
            throw std::underflow_error("LRUWithTTL: cannot evict from empty cache");

        for (const auto& pair : this->expirations) {
            if (this->isExpired(pair.first)) {
                K key = pair.first;
                this->expirations.erase(key);
                auto it = this->keyToNode[key];
                this->accessOrder.erase(it);
                this->keyToNode.erase(key);
                this->evictionCount++;
                return key;
            }
        }

        K element = this->accessOrder.back();
        this->accessOrder.pop_back();
        this->keyToNode.erase(element);
        this->expirations.erase(element);
        this->evictionCount++;
        return element;
    }

    std::string getName() const override { return "LRU+TTL"; }

    void reset() override {
        LRUPolicy<K>::reset();
        TTLPolicy<K>::reset();
    }

    template <typename KK>
    friend std::ostream& operator<<(std::ostream& os, const LRUWithTTL<KK>& obj);
};

template <typename K>
std::ostream& operator<<(std::ostream& os, const LRUWithTTL<K>& obj) {
    os << static_cast<const EvictionPolicy<K>&>(obj);
    os << "Access Order: ";
    for (const auto& key : obj.accessOrder)
        os << key << " ";
    os << "\n";
    os << "Default TTL: " << obj.defaultTTL << " seconds\n";
    os << "Active expirations: " << obj.expirations.size() << "\n";
    return os;
}
