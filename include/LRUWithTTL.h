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

    void onUpdate(const K& key) override {
        LRUPolicy<K>::onAccess(key);
        TTLPolicy<K>::onUpdate(key);
    }

    void onInsert(const K& key) override {
        if (this->keyToNode.find(key) != this->keyToNode.end())
            throw std::invalid_argument("LRUWithTTL: key already exists");
        if (this->expirations.find(key) != this->expirations.end())
            throw std::invalid_argument("LRUWithTTL: key already exists");
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

        K expiredKey{};
        bool foundExpired = false;
        for (const auto& pair : this->expirations) {
            if (this->isExpired(pair.first)) {
                expiredKey = pair.first;
                foundExpired = true;
                break;
            }
        }
        if (foundExpired) {
            this->expirations.erase(expiredKey);
            auto it = this->keyToNode.at(expiredKey);
            this->accessOrder.erase(it);
            this->keyToNode.erase(expiredKey);
            this->evictionCount++;
            return expiredKey;
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

    std::vector<K> getKeysOrdered() const override {
        return LRUPolicy<K>::getKeysOrdered();
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
