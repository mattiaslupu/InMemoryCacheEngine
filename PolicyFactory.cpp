#include "PolicyFactory.h"
#include <stdexcept>

std::unique_ptr<EvictionPolicy> PolicyFactory::create(const std::string& name) {
    if (name == "LRU")     return std::make_unique<LRUPolicy>();
    if (name == "LFU")     return std::make_unique<LFUPolicy>();
    if (name == "FIFO")    return std::make_unique<FIFOPolicy>();
    if (name == "TTL")     return std::make_unique<TTLPolicy>();
    if (name == "LRU+TTL") return std::make_unique<LRUWithTTL>();
    throw std::invalid_argument("PolicyFactory: unknown policy: " + name);
}

std::vector<std::string> PolicyFactory::availablePolicies() {
    return {"LRU", "LFU", "FIFO", "TTL", "LRU+TTL"};
}