
#include "LFUPolicy.h"

LFUPolicy::LFUPolicy() : EvictionPolicy(), minFrequency(1) {}

LFUPolicy::LFUPolicy(const LFUPolicy& obj)
    : EvictionPolicy(obj),
      frequencies{obj.frequencies},
      freqBuckets{obj.freqBuckets},
      keyToBucketNode{},
      minFrequency{obj.minFrequency} {
    for (auto& pair : freqBuckets)
        for (auto it = pair.second.begin(); it != pair.second.end(); it++)
            keyToBucketNode[*it] = it;
}

LFUPolicy& LFUPolicy::operator=(const LFUPolicy& other) {
    if (this != &other) {
        EvictionPolicy::operator=(other);
        frequencies = other.frequencies;
        freqBuckets = other.freqBuckets;
        minFrequency = other.minFrequency;
        keyToBucketNode.clear();
        for (auto& pair : freqBuckets)
            for (auto it = pair.second.begin(); it != pair.second.end(); it++)
                keyToBucketNode[*it] = it;
    }
    return *this;
}

std::string LFUPolicy::getName() const {
    return "LFU";
}

void LFUPolicy::incrementFrequency(const std::string& key) {
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

void LFUPolicy::onInsert(const std::string& key) {
    if (frequencies.find(key) != frequencies.end())
        throw std::invalid_argument("LFUPolicy: key already exists: " + key);

    frequencies[key] = 1;
    freqBuckets[1].push_front(key);
    keyToBucketNode[key] = freqBuckets[1].begin();
    minFrequency = 1;
}

void LFUPolicy::onAccess(const std::string& key) {
    if (frequencies.find(key) == frequencies.end())
        throw std::invalid_argument("LFUPolicy: key not found: " + key);
    incrementFrequency(key);
}

void LFUPolicy::onRemove(const std::string& key) {
    if (frequencies.find(key) == frequencies.end())
        throw std::invalid_argument("LFUPolicy: key not found: " + key);

    size_t freq = frequencies[key];
    freqBuckets[freq].erase(keyToBucketNode[key]);
    if (freqBuckets[freq].empty())
        freqBuckets.erase(freq);

    keyToBucketNode.erase(key);
    frequencies.erase(key);
}


std::string LFUPolicy::evict() {
    if (freqBuckets.empty())
        throw std::underflow_error("LFUPolicy: cannot evict from empty cache");

    std::string key = freqBuckets[minFrequency].back();

    freqBuckets[minFrequency].erase(keyToBucketNode[key]);
    if (freqBuckets[minFrequency].empty())
        freqBuckets.erase(minFrequency);

    frequencies.erase(key);
    keyToBucketNode.erase(key);
    evictionCount++;
    return key;
}

void LFUPolicy::reset() {
    frequencies.clear();
    freqBuckets.clear();
    keyToBucketNode.clear();
    minFrequency = 1;
    evictionCount = 0;
}

std::ostream& operator<<(std::ostream& os, const LFUPolicy& obj) {
    os << static_cast<const EvictionPolicy&>(obj);
    os << "Min Frequency: " << obj.minFrequency << "\n";
    os << "Frequencies:\n";
    for (const auto& pair : obj.frequencies)
        os << "  " << pair.first << " -> " << pair.second << "\n";
    return os;
}