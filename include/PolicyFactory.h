#pragma once
#include "LRUPolicy.h"
#include "LFUPolicy.h"
#include "FIFOPolicy.h"
#include "TTLPolicy.h"
#include "LRUWithTTL.h"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

template <typename K>
class PolicyFactory {
public:
    PolicyFactory() = delete;

    static std::unique_ptr<EvictionPolicy<K>> create(const std::string& name) {
        if (name == "LRU")     return std::make_unique<LRUPolicy<K>>();
        if (name == "LFU")     return std::make_unique<LFUPolicy<K>>();
        if (name == "FIFO")    return std::make_unique<FIFOPolicy<K>>();
        if (name == "TTL")     return std::make_unique<TTLPolicy<K>>();
        if (name == "LRU+TTL") return std::make_unique<LRUWithTTL<K>>();
        throw std::invalid_argument("PolicyFactory: unknown policy: " + name);
    }

    static std::vector<std::string> availablePolicies() {
        return {"LRU", "LFU", "FIFO", "TTL", "LRU+TTL"};
    }

    static std::vector<std::unique_ptr<EvictionPolicy<K>>> createAll() {
        std::vector<std::unique_ptr<EvictionPolicy<K>>> policies;
        for (const auto& name : availablePolicies())
            policies.push_back(create(name));
        return policies;
    }
};
