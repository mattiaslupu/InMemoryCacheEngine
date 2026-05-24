#pragma once
#include "EvictionPolicy.h"
#include <unordered_map>
#include <map>
#include <list>
#include <stdexcept>

template <typename K>
class LFUPolicy : public virtual EvictionPolicy<K> {
private:
    std::unordered_map<K, size_t> frequencies;
    std::map<size_t, std::list<K>> freqBuckets;
    std::unordered_map<K, typename std::list<K>::iterator> keyToBucketNode;
    size_t minFrequency;

    void incrementFrequency(const K& key) {
        size_t freq = frequencies[key];

        freqBuckets[freq].erase(keyToBucketNode[key]);
        if (freqBuckets[freq].empty())
            freqBuckets.erase(freq);

        frequencies[key]++;
        freqBuckets[freq + 1].push_front(key);
        keyToBucketNode[key] = freqBuckets[freq + 1].begin();

        if (freqBuckets.find(minFrequency) == freqBuckets.end())
            minFrequency++;
    }

public:
    LFUPolicy() : EvictionPolicy<K>(), minFrequency(1) {}

    LFUPolicy(const LFUPolicy& obj)
        : EvictionPolicy<K>(obj),
          frequencies{obj.frequencies},
          freqBuckets{obj.freqBuckets},
          keyToBucketNode{},
          minFrequency{obj.minFrequency} {
        for (auto& pair : freqBuckets)
            for (auto it = pair.second.begin(); it != pair.second.end(); ++it)
                keyToBucketNode[*it] = it;
    }

    LFUPolicy& operator=(const LFUPolicy& other) {
        if (this != &other) {
            EvictionPolicy<K>::operator=(other);
            frequencies = other.frequencies;
            freqBuckets = other.freqBuckets;
            minFrequency = other.minFrequency;
            keyToBucketNode.clear();
            for (auto& pair : freqBuckets)
                for (auto it = pair.second.begin(); it != pair.second.end(); ++it)
                    keyToBucketNode[*it] = it;
        }
        return *this;
    }

    LFUPolicy(LFUPolicy&& other) noexcept = default;
    LFUPolicy& operator=(LFUPolicy&& other) noexcept = default;
    ~LFUPolicy() = default;

    void onInsert(const K& key) override {
        if (frequencies.find(key) != frequencies.end())
            throw std::invalid_argument("LFUPolicy: key already exists");
        frequencies[key] = 1;
        freqBuckets[1].push_front(key);
        keyToBucketNode[key] = freqBuckets[1].begin();
        minFrequency = 1;
    }

    void onAccess(const K& key) override {
        if (frequencies.find(key) == frequencies.end())
            throw std::invalid_argument("LFUPolicy: key not found");
        incrementFrequency(key);
    }

    void onRemove(const K& key) override {
        if (frequencies.find(key) == frequencies.end())
            throw std::invalid_argument("LFUPolicy: key not found");
        size_t freq = frequencies.at(key);
        freqBuckets.at(freq).erase(keyToBucketNode.at(key));
        if (freqBuckets.at(freq).empty()) {
            freqBuckets.erase(freq);
            if (freq == minFrequency && !freqBuckets.empty())
                minFrequency = freqBuckets.begin()->first;
        }
        keyToBucketNode.erase(key);
        frequencies.erase(key);
    }

    void onUpdate(const K& key) override {
        if (frequencies.find(key) == frequencies.end())
            throw std::invalid_argument("LFUPolicy: key not found");
        // writes do not increment frequency in LFU
    }

    K evict() override {
        if (freqBuckets.empty())
            throw std::underflow_error("LFUPolicy: cannot evict from empty cache");
        K key = freqBuckets.at(minFrequency).back();
        freqBuckets.at(minFrequency).erase(keyToBucketNode.at(key));
        if (freqBuckets.at(minFrequency).empty())
            freqBuckets.erase(minFrequency);
        frequencies.erase(key);
        keyToBucketNode.erase(key);
        this->evictionCount++;
        return key;
    }

    std::string getName() const override { return "LFU"; }

    void reset() override {
        frequencies.clear();
        freqBuckets.clear();
        keyToBucketNode.clear();
        minFrequency = 1;
        this->evictionCount = 0;
    }

    std::vector<K> getKeysOrdered() const override {
        std::vector<K> result;
        for (const auto& pair : freqBuckets)
            for (auto it = pair.second.rbegin(); it != pair.second.rend(); ++it)
                result.push_back(*it);
        return result;
    }

    template <typename KK>
    friend std::ostream& operator<<(std::ostream& os, const LFUPolicy<KK>& obj);
};

template <typename K>
std::ostream& operator<<(std::ostream& os, const LFUPolicy<K>& obj) {
    os << static_cast<const EvictionPolicy<K>&>(obj);
    os << "Min Frequency: " << obj.minFrequency << "\n";
    os << "Frequencies:\n";
    for (const auto& pair : obj.frequencies)
        os << "  " << pair.first << " -> " << pair.second << "\n";
    return os;
}
