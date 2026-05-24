#pragma once
#include "Cache.h"
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

template <typename T>
void printCollection(const std::vector<T>& collection) {
    for (const auto& item : collection)
        std::cout << item << "\n";
}

template <typename K, typename V>
void populateCache(Cache<K, V>& cache, const std::vector<std::pair<K, V>>& data) {
    for (const auto& pair : data)
        cache.put(pair.first, pair.second);
}

template <typename K, typename V>
void forEachHit(Cache<K, V>& cache,
                const std::vector<K>& keys,
                std::function<void(const K&, const V&)> onHit) {
    for (const auto& key : keys) {
        auto result = cache.tryGet(key);
        if (result)
            onHit(key, *result);
    }
}

template <typename T>
typename std::vector<T>::const_iterator findFirst(const std::vector<T>& collection,
                                                   std::function<bool(const T&)> predicate) {
    return std::find_if(collection.begin(), collection.end(), predicate);
}
