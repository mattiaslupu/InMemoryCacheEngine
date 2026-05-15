#pragma once
#include "EvictionPolicy.h"
#include <list>
#include <unordered_map>
#include <string>

class LRUPolicy : public virtual EvictionPolicy {
private:
    std::list<std::string> accessOrder;
    std::unordered_map<std::string, std::list<std::string>::iterator> keyToNode;

    void moveToFront(const std::string& key);

public:
    LRUPolicy();
    LRUPolicy(const LRUPolicy& obj);

    LRUPolicy& operator=(const LRUPolicy& obj);

    ~LRUPolicy() override;

    void onAccess(const std::string& key) override;
    void onInsert(const std::string& key) override;
    void onRemove(const std::string& key) override;
    std::string evict() override;
    std::string getName() const override;
    void reset() override;
};