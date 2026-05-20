#pragma once
#include "EvictionPolicy.h"
#include <unordered_map>
#include <string>
#include <ctime>

class TTLPolicy : public virtual EvictionPolicy {
protected:
    std::unordered_map<std::string, time_t> expirations;
    time_t defaultTTL;

public:
    TTLPolicy();
    explicit TTLPolicy(time_t defaultTTL);
    TTLPolicy(const TTLPolicy& obj) = default;
    TTLPolicy(TTLPolicy&& obj) noexcept = default;
    TTLPolicy& operator=(const TTLPolicy& obj) = default;
    TTLPolicy& operator=(TTLPolicy&& obj) noexcept = default;
    ~TTLPolicy() = default;

    void onAccess(const std::string& key) override;
    void onInsert(const std::string& key) override;
    void onRemove(const std::string& key) override;
    std::string evict() override;
    std::string getName() const override;
    void reset() override;

    bool isExpired(const std::string& key) const;

    void setDefaultTTL(time_t seconds);
    time_t getDefaultTTL() const;
    friend std::ostream& operator<<(std::ostream& os, const TTLPolicy& obj);
};