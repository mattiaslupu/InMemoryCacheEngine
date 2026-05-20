#pragma once
#include "EvictionPolicy.h"
#include "LRUPolicy.h"
#include "LFUPolicy.h"
#include "FIFOPolicy.h"
#include "TTLPolicy.h"
#include "LRUWithTTL.h"
#include <memory>
#include <string>
#include <vector>

class PolicyFactory {
public:
    PolicyFactory() = delete;
    static std::unique_ptr<EvictionPolicy> create(const std::string& name);
    static std::vector<std::string> availablePolicies();
};