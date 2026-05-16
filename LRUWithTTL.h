#pragma once
#include "LRUPolicy.h"
#include "TTLPolicy.h"

class LRUWithTTL : public LRUPolicy, public TTLPolicy {
public:
    LRUWithTTL();
    explicit LRUWithTTL(time_t defaultTTL);
    LRUWithTTL(const LRUWithTTL& obj);

    LRUWithTTL& operator=(const LRUWithTTL& obj);

    ~LRUWithTTL() override;

    void onAccess(const std::string& key) override;

    void onInsert(const std::string& key) override;

    void onRemove(const std::string& key) override;

    std::string evict() override;

    std::string getName() const override;

    void reset() override;

    friend std::ostream& operator<<(std::ostream& os, const LRUWithTTL& obj);
};