#pragma once
#include <string>
#include <iostream>

template <typename K>
class EvictionPolicy {
protected:
    std::size_t evictionCount;

public:
    EvictionPolicy() : evictionCount(0) {}
    EvictionPolicy(const EvictionPolicy& other) = default;
    EvictionPolicy(EvictionPolicy&& other) noexcept = default;
    EvictionPolicy& operator=(const EvictionPolicy& other) = default;
    EvictionPolicy& operator=(EvictionPolicy&& other) noexcept = default;
    virtual ~EvictionPolicy() = default;

    virtual void onAccess(const K& key) = 0;
    virtual void onInsert(const K& key) = 0;
    virtual void onRemove(const K& key) = 0;
    virtual K evict() = 0;
    virtual std::string getName() const = 0;
    virtual void reset() = 0;

    size_t getEvictionCount() const { return evictionCount; }

    template <typename KK>
    friend std::ostream& operator<<(std::ostream& os, const EvictionPolicy<KK>& obj);
};

template <typename K>
std::ostream& operator<<(std::ostream& os, const EvictionPolicy<K>& obj) {
    os << "Eviction Count: " << obj.evictionCount << "\n";
    return os;
}
