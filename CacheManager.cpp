#include "CacheManager.h"


CacheManager::CacheManager() {

}

CacheManager::~CacheManager() {}

CacheManager& CacheManager::getInstance() {
    static CacheManager instance;
    return instance;
}

void CacheManager::registerCache(const std::string& name, std::shared_ptr<Cache<std::string, std::string>> cache) {
    if (name.empty())
        throw std::invalid_argument("CacheManager: name cannot be empty");
    if (!cache)
        throw std::invalid_argument("CacheManager: cache cannot be null");
    if (hasCache(name))
        throw std::invalid_argument("CacheManager: cache already exists: " + name);
    caches[name] = cache;
}

std::shared_ptr<Cache<std::string, std::string>> CacheManager::getCache(const std::string& name) {
    if (!hasCache(name))
        throw std::out_of_range("CacheManager: cache not found: " + name);
    return caches.at(name);
}

void CacheManager::removeCache(const std::string& name) {
    if (!hasCache(name))
        throw std::out_of_range("CacheManager: cache not found: " + name);
    caches.erase(name);
}

bool CacheManager::hasCache(const std::string& name) const {
    return caches.find(name) != caches.end();
}

size_t CacheManager::count() const {
    return caches.size();
}

std::vector<std::string> CacheManager::listCacheNames() const {
    std::vector<std::string> names;
    for (const auto& pair : caches)
        names.push_back(pair.first);
    return names;
}

void CacheManager::displayAll() const {
    if (caches.empty()) {
        std::cout << "No caches registered.\n";
        return;
    }
    for (const auto& pair : caches) {
        std::cout << "=== Cache: " << pair.first << " ===\n";
        std::cout << *pair.second << "\n";
    }
}

std::ostream& operator<<(std::ostream& os, const CacheManager& manager) {
    os << "CacheManager: " << manager.caches.size() << " cache(s)\n";
    for (const auto& pair : manager.caches)
        os << "  [" << pair.first << "] → " << pair.second->getPolicyName()
           << " (" << pair.second->size() << "/" << pair.second->getCapacity() << ")\n";
    return os;
}