#include "CacheManager.h"


CacheManager& CacheManager::getInstance() {
    static CacheManager instance;
    return instance;
}


void CacheManager::registerCache(const std::string& name,
                                  std::shared_ptr<Cache<std::string, std::string>> cache) {
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
    auto locked = caches.at(name).lock();
    if (!locked)
        throw std::runtime_error("CacheManager: cache has been destroyed: " + name);
    return locked;
}

void CacheManager::removeCache(const std::string& name) {
    if (!hasCache(name))
        throw std::out_of_range("CacheManager: cache not found: " + name);
    caches.erase(name);
}

bool CacheManager::hasCache(const std::string& name) const {
    auto it = caches.find(name);
    if (it == caches.end())
        return false;
    return !it->second.expired();
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
        auto locked = pair.second.lock();
        if (locked)
            std::cout << "=== Cache: " << pair.first << " ===\n" << *locked << "\n";
        else
            std::cout << "=== Cache: " << pair.first << " === [destroyed]\n";
    }
}

std::ostream& operator<<(std::ostream& os, const CacheManager& manager) {
    os << "CacheManager: " << manager.caches.size() << " cache(s)\n";
    for (const auto& pair : manager.caches) {
        auto locked = pair.second.lock();
        if (locked)
            os << "  [" << pair.first << "] -> " << locked->getPolicyName()
               << " (" << locked->size() << "/" << locked->getCapacity() << ")\n";
        else
            os << "  [" << pair.first << "] -> [destroyed]\n";
    }
    return os;
}