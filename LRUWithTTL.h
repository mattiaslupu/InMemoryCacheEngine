#pragma once
#include "LRUPolicy.h"
#include "TTLPolicy.h"

class LRUWithTTL : public LRUPolicy, public TTLPolicy {
public:
    LRUWithTTL() = default;
    explicit LRUWithTTL(time_t defaultTTL);
    LRUWithTTL(const LRUWithTTL& other);
    LRUWithTTL(LRUWithTTL&& other) noexcept = default;
    LRUWithTTL& operator=(const LRUWithTTL& other);
    LRUWithTTL& operator=(LRUWithTTL&& other) noexcept = default;
    ~LRUWithTTL() = default;

    void onAccess(const std::string& key) override;
    void onInsert(const std::string& key) override;
    void onRemove(const std::string& key) override;
    std::string evict() override;
    std::string getName() const override;
    void reset() override;
    friend std::ostream& operator<<(std::ostream& os, const LRUWithTTL& obj);
};