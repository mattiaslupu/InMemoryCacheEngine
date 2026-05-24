#pragma once
#include "ICache.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

class CacheManager {
private:
    std::unordered_map<std::string, std::weak_ptr<ICache>> caches;

    CacheManager() = default;

public:
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;
    CacheManager(CacheManager&&) = delete;
    CacheManager& operator=(CacheManager&&) = delete;

    ~CacheManager() = default;

    static CacheManager& getInstance();

    void registerCache(const std::string& name, std::shared_ptr<ICache> cache);

    std::shared_ptr<ICache> getCache(const std::string& name);

    void removeCache(const std::string& name);

    bool hasCache(const std::string& name) const;

    size_t count() const;

    std::vector<std::string> listCacheNames() const;

    void displayAll() const;

    friend std::ostream& operator<<(std::ostream& os, const CacheManager& manager);
};
