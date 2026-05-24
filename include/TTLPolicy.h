#pragma once
#include "EvictionPolicy.h"
#include <unordered_map>
#include <ctime>
#include <limits>
#include <stdexcept>

template <typename K>
class TTLPolicy : public virtual EvictionPolicy<K> {
protected:
    std::unordered_map<K, time_t> expirations;
    time_t defaultTTL;

public:
    TTLPolicy() : EvictionPolicy<K>(), defaultTTL{60} {}

    explicit TTLPolicy(time_t defaultTTL) : EvictionPolicy<K>(), defaultTTL{defaultTTL} {
        if (defaultTTL < 0)
            throw std::invalid_argument("TTLPolicy: TTL cannot be negative");
    }

    TTLPolicy(const TTLPolicy& obj) = default;
    TTLPolicy(TTLPolicy&& obj) noexcept = default;
    TTLPolicy& operator=(const TTLPolicy& obj) = default;
    TTLPolicy& operator=(TTLPolicy&& obj) noexcept = default;
    ~TTLPolicy() = default;

    void onAccess([[maybe_unused]] const K& key) override {}

    void onInsert(const K& key) override {
        if (expirations.find(key) != expirations.end())
            throw std::invalid_argument("TTLPolicy: key already exists");
        time_t now = std::time(nullptr);
        expirations[key] = (defaultTTL > std::numeric_limits<time_t>::max() - now)
                           ? std::numeric_limits<time_t>::max()
                           : now + defaultTTL;
    }

    void onUpdate(const K& key) override {
        if (expirations.find(key) == expirations.end())
            throw std::invalid_argument("TTLPolicy: key not found");
        time_t now = std::time(nullptr);
        expirations[key] = (defaultTTL > std::numeric_limits<time_t>::max() - now)
                           ? std::numeric_limits<time_t>::max()
                           : now + defaultTTL;
    }

    void onRemove(const K& key) override {
        if (expirations.find(key) == expirations.end())
            throw std::invalid_argument("TTLPolicy: key not found");
        expirations.erase(key);
    }

    K evict() override {
        if (expirations.empty())
            throw std::underflow_error("TTLPolicy: cannot evict from empty cache");
        K found{};
        bool hasExpired = false;
        for (const auto& pair : expirations) {
            if (isExpired(pair.first)) {
                found = pair.first;
                hasExpired = true;
                break;
            }
        }
        if (hasExpired) {
            expirations.erase(found);
            this->evictionCount++;
            return found;
        }
        K key = expirations.begin()->first;
        expirations.erase(key);
        this->evictionCount++;
        return key;
    }

    std::string getName() const override { return "TTL"; }

    void reset() override {
        expirations.clear();
        this->evictionCount = 0;
    }

    std::vector<K> getKeysOrdered() const override {
        std::vector<K> result;
        for (const auto& pair : expirations)
            result.push_back(pair.first);
        return result;
    }

    bool isExpired(const K& key) const {
        if (expirations.find(key) == expirations.end())
            return false;
        return std::time(nullptr) > expirations.at(key);
    }

    void setDefaultTTL(time_t seconds) override {
        if (seconds < 0)
            throw std::invalid_argument("TTLPolicy: TTL cannot be negative");
        defaultTTL = seconds;
    }

    time_t getDefaultTTL() const override { return defaultTTL; }

    template <typename KK>
    friend std::ostream& operator<<(std::ostream& os, const TTLPolicy<KK>& obj);
};

template <typename K>
std::ostream& operator<<(std::ostream& os, const TTLPolicy<K>& obj) {
    os << static_cast<const EvictionPolicy<K>&>(obj);
    os << "Default TTL: " << obj.defaultTTL << " seconds\n";
    os << "Active keys: " << obj.expirations.size() << "\n";
    return os;
}
