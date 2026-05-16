#pragma once
#include "EvictionPolicy.h"
#include <unordered_map>
#include <map>
#include <list>
#include <string>

class LFUPolicy : public virtual EvictionPolicy {
private:
    std::unordered_map<std::string, size_t> frequencies;
    std::map<size_t, std::list<std::string>> freqBuckets;
    std::unordered_map<std::string, std::list<std::string>::iterator> keyToBucketNode;
    size_t minFrequency;

    void incrementFrequency(const std::string& key);

public:
    LFUPolicy();
    LFUPolicy(const LFUPolicy& obj);

    LFUPolicy& operator=(const LFUPolicy& other);

    ~LFUPolicy() override;

    void onAccess(const std::string& key) override;
    void onInsert(const std::string& key) override;
    void onRemove(const std::string& key) override;
    std::string evict() override;
    std::string getName() const override;
    void reset() override;
    friend std::ostream& operator<<(std::ostream& os, const LFUPolicy& obj);
};