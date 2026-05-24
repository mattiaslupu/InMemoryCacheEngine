#pragma once
#include "ICache.h"
#include "CacheEntry.h"
#include "EvictionPolicy.h"
#include "Statistics.h"
#include "LRUPolicy.h"
#include "PolicyFactory.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <optional>
#include "CacheException.h"

template <typename K, typename V>
class Cache : public ICache {
    static_assert(std::is_copy_constructible<K>::value,
       "Cache: key type must be copy constructible");
    static_assert(std::is_copy_constructible<V>::value,
        "Cache: value type must be copy constructible");
private:
    std::unordered_map<K, CacheEntry<V>> store;
    std::unique_ptr<EvictionPolicy<K>> policy;
    Statistics stats;
    size_t capacity;
    const int id;
    static int noInstances;

    void evictOne();
    void cleanExpired();

    static int nextId(size_t cap, const std::unique_ptr<EvictionPolicy<K>>& pol) {
        if (cap == 0) throw InvalidCapacityException();
        if (!pol)     throw PolicyPtrException();
        return ++noInstances;
    }

public:
    Cache();
    Cache(size_t capacity, std::unique_ptr<EvictionPolicy<K>> policy);
    Cache(const Cache<K, V>& other);
    Cache<K, V>& operator=(const Cache<K, V>& other);
    Cache(Cache<K,V>&& other) noexcept = default;
    Cache<K,V>& operator=(Cache<K,V>&& other) noexcept = default;
    ~Cache();

    void put(const K& key, const V& value);
    V* get(const K& key);
    std::optional<V> tryGet(const K& key);
    void switchPolicy(std::unique_ptr<EvictionPolicy<K>> newPolicy);
    void resize(size_t newCapacity);
    bool remove(const K& key);
    void clear();

    size_t size() const override;
    size_t getCapacity() const override;
    const Statistics& getStats() const;
    std::string getPolicyName() const override;
    void print(std::ostream& os) const override;
    int getId() const;
    static int getNoInstances();

    template <typename KK, typename VV>
    friend std::ostream& operator<<(std::ostream& os, const Cache<KK, VV>& cache);

    template <typename KK, typename VV>
    friend std::istream& operator>>(std::istream& is, Cache<KK, VV>& cache);
};

template <typename K, typename V>
int Cache<K, V>::noInstances = 0;

template <typename K, typename V>
Cache<K, V>::Cache()
    : store{},
      policy{std::make_unique<LRUPolicy<K>>()},
      stats{},
      capacity{10},
      id{++noInstances} {
    store.reserve(10);
}

template <typename K, typename V>
Cache<K, V>::Cache(size_t capacity, std::unique_ptr<EvictionPolicy<K>> policy)
    : store{},
      policy{std::move(policy)},
      stats{},
      capacity{capacity},
      id{nextId(capacity, this->policy)} {
    store.reserve(capacity);
}

template <typename K, typename V>
Cache<K, V>::Cache(const Cache<K, V>& other)
    : store{},
      policy{PolicyFactory<K>::create(other.policy->getName())},
      stats{},
      capacity{other.capacity},
      id{++noInstances} {
    store.reserve(capacity);
    policy->setDefaultTTL(other.policy->getDefaultTTL());
    for (const K& key : other.policy->getKeysOrdered()) {
        const auto& entry = other.store.at(key);
        if (!entry.isExpired()) {
            store[key] = entry;
            policy->onInsert(key);
            stats.recordInsertion();
        }
    }
}

template <typename K, typename V>
Cache<K, V>& Cache<K, V>::operator=(const Cache<K, V>& other) {
    if (this != &other) {
        store.clear();
        policy = PolicyFactory<K>::create(other.policy->getName());
        policy->setDefaultTTL(other.policy->getDefaultTTL());
        stats.reset();
        capacity = other.capacity;
        store.reserve(capacity);
        for (const K& key : other.policy->getKeysOrdered()) {
            const auto& entry = other.store.at(key);
            if (!entry.isExpired()) {
                store[key] = entry;
                policy->onInsert(key);
                stats.recordInsertion();
            }
        }
    }
    return *this;
}

template <typename K, typename V>
Cache<K, V>::~Cache() {}

template <typename K, typename V>
void Cache<K, V>::print(std::ostream& os) const {
    os << "Capacity: " << store.size() << "/" << capacity << "\n";
    os << "Policy: " << policy->getName() << "\n";
    os << "Contents:\n";
    for (const auto& pair : store)
        os << "  [" << pair.first << "] -> " << pair.second << "\n";
    os << stats;
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const Cache<K, V>& cache) {
    cache.print(os);
    return os;
}

template <typename K, typename V>
std::istream& operator>>(std::istream& is, Cache<K, V>& cache) {
    K key;
    std::cout << "Introduce the key: ";
    if (!(is >> key))
        return is;
    V* result = cache.get(key);
    if (result != nullptr)
        std::cout << "Found: " << *result << "\n";
    else
        std::cout << "The '" << key << "' key does not exist in the cache.\n";
    return is;
}

template<typename K, typename V>
void Cache<K, V>::evictOne() {
    K key = policy->evict();
    store.erase(key);
    stats.recordEviction();
}

template<typename K, typename V>
void Cache<K, V>::cleanExpired() {
    std::vector<K> expired;

    for (const auto& pair : store)
        if (pair.second.isExpired())
            expired.push_back(pair.first);

    for (const K& key : expired) {
        policy->onRemove(key);
        store.erase(key);
        stats.recordEviction();
    }
}

template<typename K, typename V>
void Cache<K, V>::put(const K& key, const V& value) {
    if (store.find(key) != store.end()) {
        store[key].setValue(value);
        store[key].refreshInsertionTime();
        store[key].touch();
        policy->onUpdate(key);
        return;
    }
    if (store.size() == capacity) {
        cleanExpired();
        if (store.size() == capacity)
            evictOne();
    }
    store[key] = CacheEntry<V>(value, policy->getDefaultTTL());
    policy->onInsert(key);
    stats.recordInsertion();
}

template<typename K, typename V>
V* Cache<K, V>::get(const K& key) {
    if (store.find(key) == store.end()) {
        stats.recordMiss();
        return nullptr;
    }
    if (store[key].isExpired()) {
        policy->onRemove(key);
        store.erase(key);
        stats.recordEviction();
        stats.recordMiss();
        return nullptr;
    }
    store[key].touch();
    policy->onAccess(key);
    stats.recordHit();
    return &store[key].getValue();
}

template<typename K, typename V>
void Cache<K, V>::switchPolicy(std::unique_ptr<EvictionPolicy<K>> newPolicy) {
    if (!newPolicy)
        throw PolicyPtrException();
    cleanExpired();
    newPolicy->reset();
    for (const K& key : policy->getKeysOrdered())
        newPolicy->onInsert(key);
    policy = std::move(newPolicy);
}

template<typename K, typename V>
void Cache<K, V>::resize(size_t newCapacity) {
    if (newCapacity == 0)
        throw InvalidCapacityException();
    while (store.size() > newCapacity)
        evictOne();
    store.reserve(newCapacity);
    capacity = newCapacity;
}

template<typename K, typename V>
bool Cache<K, V>::remove(const K& key) {
    if (store.find(key) == store.end())
        return false;
    policy->onRemove(key);
    store.erase(key);
    stats.recordEviction();
    return true;
}

template<typename K, typename V>
void Cache<K, V>::clear() {
    store.clear();
    policy->reset();
    stats.reset();
}

template <typename K, typename V>
size_t Cache<K, V>::size() const {
    return store.size();
}

template <typename K, typename V>
size_t Cache<K, V>::getCapacity() const {
    return capacity;
}

template <typename K, typename V>
const Statistics& Cache<K, V>::getStats() const {
    return stats;
}

template <typename K, typename V>
std::string Cache<K, V>::getPolicyName() const {
    return policy->getName();
}

template <typename K, typename V>
int Cache<K, V>::getId() const {
    return id;
}

template <typename K, typename V>
int Cache<K, V>::getNoInstances() {
    return noInstances;
}

template <typename K, typename V>
std::optional<V> Cache<K, V>::tryGet(const K& key) {
    V* result = get(key);
    if (result)
        return *result;
    return std::nullopt;
}
