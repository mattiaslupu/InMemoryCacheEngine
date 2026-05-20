#pragma once
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
#include "CacheException.h"

template <typename K, typename V>
class Cache {
    static_assert(std::is_copy_constructible<K>::value,
       "Cache: key type must be copy constructible");
    static_assert(std::is_copy_constructible<V>::value,
        "Cache: value type must be copy constructible");
private:
    std::unordered_map<K, CacheEntry<V>> store;
    std::unique_ptr<EvictionPolicy> policy;
    Statistics stats;
    size_t capacity;
    const int id;
    static int noInstances;

    void evictOne();
    void cleanExpired();

public:
    Cache();
    Cache(size_t capacity, std::unique_ptr<EvictionPolicy> policy);
    Cache(const Cache<K, V>& other);
    Cache<K, V>& operator=(const Cache<K, V>& other);
    Cache(Cache<K,V>&& other) noexcept = default;
    Cache<K,V>& operator=(Cache<K,V>&& other) noexcept = default;
    ~Cache();

    void put(const K& key, const V& value);
    V* get(const K& key);
    void switchPolicy(std::unique_ptr<EvictionPolicy> newPolicy);
    void resize(size_t newCapacity);
    bool remove(const K& key);
    void clear();

    size_t size() const;
    size_t getCapacity() const;
    const Statistics& getStats() const;
    std::string getPolicyName() const;
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
      policy{std::make_unique<LRUPolicy>()},
      stats{},
      capacity{10},
      id{++noInstances} {}

template <typename K, typename V>
Cache<K, V>::Cache(size_t capacity, std::unique_ptr<EvictionPolicy> policy)
    : store{},
      policy{std::move(policy)},
      stats{},
      capacity{capacity},
      id{++noInstances} {
    if (capacity == 0)
        throw InvalidCapacityException();
    if (!this->policy)
        throw PolicyPtrException();
}

template <typename K, typename V>
Cache<K, V>::Cache(const Cache<K, V>& other)
    : store{other.store},
      policy{PolicyFactory::create(other.policy->getName())},
      stats{other.stats},
      capacity{other.capacity},
      id{++noInstances} {
    for (const auto& pair : store)
        policy->onInsert(pair.first);
}

template <typename K, typename V>
Cache<K, V>& Cache<K, V>::operator=(const Cache<K, V>& other) {
    if (this != &other) {
        store = other.store;
        policy = PolicyFactory::create(other.policy->getName());
        stats = other.stats;
        capacity = other.capacity;
        for (const auto& pair : store)
            policy->onInsert(pair.first);
    }
    return *this;
}

template <typename K, typename V>
Cache<K, V>::~Cache() {}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const Cache<K, V>& cache) {
    os << "Capacity: " << cache.store.size() << "/" << cache.capacity << "\n";
    os << "Policy: " << cache.policy->getName() << "\n";
    os << "Contents:\n";
    for (const auto& pair : cache.store)
        os << "  [" << pair.first << "] -> " << pair.second << "\n";
    os << cache.stats;
    return os;
}

template <typename K, typename V>
std::istream& operator>>(std::istream& is, Cache<K, V>& cache) {
    K key;
    std::cout << "Introduce the key: ";
    if (!(is >> key))
        throw KeyNotFoundException(key);
    V* result = cache.get(key);
    if (result != nullptr)
        std::cout << "Found: " << *result << "\n";
    else
        std::cout << "The '" << key << "' key does not exist in the cache.\n";
    return is;
}


template<typename K, typename V>
void Cache<K, V>::evictOne() {
    std::string key = policy->evict();
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
        store[key].touch();
        policy->onAccess(key);
        return;
    }
    if (store.size() == capacity)
        evictOne();
    store[key] = CacheEntry<V>(value);
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
        stats.recordMiss();
        return nullptr;
    }
    store[key].touch();
    policy->onAccess(key);
    stats.recordHit();
    return &store[key].getValue();
}

template<typename K, typename V>
void Cache<K, V>::switchPolicy(std::unique_ptr<EvictionPolicy> newPolicy) {
    newPolicy->reset();
    for (const auto& pair : store)
        newPolicy->onInsert(pair.first);
    policy = std::move(newPolicy);
}

template<typename K, typename V>
void Cache<K, V>::resize(size_t newCapacity) {
    if (newCapacity == 0)
        throw InvalidCapacityException();
    while (store.size() > newCapacity)
        evictOne();
    capacity = newCapacity;
}

template<typename K, typename V>
bool Cache<K, V>::remove(const K& key) {
    if (store.find(key) == store.end())
        return false;
    policy->onRemove(key);
    store.erase(key);
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
