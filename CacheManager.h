#pragma once
#include "Cache.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

class CacheManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Cache<std::string, std::string>>> caches;

    CacheManager();

public:
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;
    CacheManager(CacheManager&&) = delete;
    CacheManager& operator=(CacheManager&&) = delete;

    ~CacheManager();

    static CacheManager& getInstance();

    void registerCache(const std::string& name, std::shared_ptr<Cache<std::string, std::string>> cache);

    std::shared_ptr<Cache<std::string, std::string>> getCache(const std::string& name);

    void removeCache(const std::string& name);

    bool hasCache(const std::string& name) const;

    size_t count() const;

    std::vector<std::string> listCacheNames() const;

    void displayAll() const;

    friend std::ostream& operator<<(std::ostream& os, const CacheManager& manager);
};