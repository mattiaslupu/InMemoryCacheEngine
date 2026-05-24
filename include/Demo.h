#pragma once
#include "Cache.h"
#include "PolicyFactory.h"
#include "Utils.h"
#include <iostream>
#include <functional>

inline void demoGenericCache() {
    std::cout << "\n[Template Demo] Cache<int, std::string>\n";
    Cache<int, std::string> intCache(5, PolicyFactory<int>::create("LRU"));
    populateCache(intCache, {{1, "one"}, {2, "two"}, {3, "three"}});

    std::vector<int> keys = {1, 2, 99};
    forEachHit(intCache, keys, std::function<void(const int&, const std::string&)>(
        [](const int& k, const std::string& v) {
            std::cout << "  hit: [" << k << "] -> \"" << v << "\"\n";
        }));

    auto result = intCache.tryGet(99);
    std::cout << "  tryGet(99): " << (result ? *result : "not found") << "\n";

    std::cout << "\n[Template Demo] Cache<int, int>\n";
    Cache<int, int> intIntCache(3, PolicyFactory<int>::create("FIFO"));
    populateCache(intIntCache, {{10, 100}, {20, 200}, {30, 300}});

    auto val = intIntCache.tryGet(20);
    std::cout << "  tryGet(20): " << (val ? std::to_string(*val) : "not found") << "\n";

    std::vector<std::string> policyNames = PolicyFactory<int>::availablePolicies();
    auto it = findFirst(policyNames, std::function<bool(const std::string&)>(
        [](const std::string& name) { return name == "LFU"; }));
    if (it != policyNames.end())
        std::cout << "  findFirst \"LFU\" in policies: found\n";

    std::cout << "\n";
}
