#pragma once
#include <string>
#include <iostream>

class EvictionPolicy {
protected:
    std::size_t evictionCount;

public:
    EvictionPolicy();
    EvictionPolicy(const EvictionPolicy& other) = default;
    EvictionPolicy(EvictionPolicy&& other) noexcept = default;
    EvictionPolicy& operator=(const EvictionPolicy& other) = default;
    EvictionPolicy& operator=(EvictionPolicy&& other) noexcept = default;
    virtual ~EvictionPolicy() = default;

    virtual void onAccess(const std::string& key) = 0;
    virtual void onInsert(const std::string& key) = 0;
    virtual void onRemove(const std::string& key) = 0;
    virtual std::string evict() = 0;
    virtual std::string getName() const = 0;
    virtual void reset() = 0;

    size_t getEvictionCount() const;

    friend std::ostream& operator<<(std::ostream& os, const EvictionPolicy& obj);
};