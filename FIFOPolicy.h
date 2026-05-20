#pragma once
#include "EvictionPolicy.h"
#include "FIFOPolicy.h"
#include <queue>
#include <string>
#include <unordered_set>

class FIFOPolicy : public virtual EvictionPolicy {
private:
    std::queue<std::string> insertionOrder;
    std::unordered_set<std::string> removed;
public:
    FIFOPolicy() = default;
    FIFOPolicy(const FIFOPolicy& other) = default;
    FIFOPolicy(FIFOPolicy&& other) noexcept = default;
    FIFOPolicy& operator=(const FIFOPolicy& other) = default;
    FIFOPolicy& operator=(FIFOPolicy&& other) noexcept = default;
    ~FIFOPolicy() = default;

    void onAccess(const std::string& key) override;
    void onInsert(const std::string& key) override;
    void onRemove(const std::string& key) override;
    std::string evict() override;
    std::string getName() const override;
    void reset() override;
    friend std::ostream& operator<<(std::ostream& os, const FIFOPolicy & obj);
};