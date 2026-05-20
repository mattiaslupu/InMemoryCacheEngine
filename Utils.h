#pragma once
#include <iostream>
#include <vector>

template <typename T>
void printCollection(const std::vector<T>& collection) {
    for (const auto& item : collection)
        std::cout << item << "\n";
}

template <typename K, typename V>
void populateCache(Cache<K,V>& cache, const std::vector<std::pair<K,V>>& data) {
    for (const auto& pair : data)
        cache.put(pair.first, pair.second);
}