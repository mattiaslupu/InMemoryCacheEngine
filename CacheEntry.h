#pragma once
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <limits>

template <typename V>
class CacheEntry {
private:
    V value;
    std::time_t timestamp;
    std::time_t insertionTime;
    std::size_t frequency;
    std::time_t ttl;

    static std::time_t currentTime() {
        std::time_t now = std::time(nullptr);
        if (now == static_cast<std::time_t>(-1))
            throw std::runtime_error("CacheEntry: system time unavailable");
        return now;
    }

public:
    CacheEntry();
    CacheEntry(const V& value, time_t ttl = 0);
    CacheEntry(const CacheEntry<V>& obj);
    CacheEntry<V>& operator=(const CacheEntry<V>& obj);
    ~CacheEntry();

    V& getValue() { return value; }
    const V& getValue() const { return value; }
    time_t getTimestamp() const;
    time_t getInsertionTime() const;
    size_t getFrequency() const;
    time_t getTTL() const;

    void setValue(const V& v);
    void setTTL(time_t t);

    void touch();
    bool isExpired() const;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const CacheEntry<U>& entry);
};

template <typename V>
CacheEntry<V>::CacheEntry()
    : value{},
      timestamp{currentTime()},
      insertionTime{currentTime()},
      frequency{0},
      ttl{0} {}

template <typename V>
CacheEntry<V>::CacheEntry(const V& value, time_t ttl)
    : value{value},
      timestamp{currentTime()},
      insertionTime{currentTime()},
      frequency{0},
      ttl{0} {
    if (ttl < 0)
        throw std::invalid_argument("CacheEntry: TTL cannot be negative");
    this->ttl = ttl;
}

template <typename V>
CacheEntry<V>::CacheEntry(const CacheEntry<V>& obj)
    : value{obj.value},
      timestamp{obj.timestamp},
      insertionTime{obj.insertionTime},
      frequency{obj.frequency},
      ttl{obj.ttl} {}

template <typename V>
CacheEntry<V>& CacheEntry<V>::operator=(const CacheEntry<V>& obj) {
    if (this != &obj) {
        value = obj.value;
        timestamp = obj.timestamp;
        insertionTime = obj.insertionTime;
        frequency = obj.frequency;
        ttl = obj.ttl;
    }
    return *this;
}

template <typename V>
CacheEntry<V>::~CacheEntry() {}



template <typename V>
time_t CacheEntry<V>::getTimestamp() const {
    return timestamp;
}

template <typename V>
time_t CacheEntry<V>::getInsertionTime() const {
    return insertionTime;
}

template <typename V>
size_t CacheEntry<V>::getFrequency() const {
    return frequency;
}

template <typename V>
time_t CacheEntry<V>::getTTL() const {
    return ttl;
}

template <typename V>
void CacheEntry<V>::setValue(const V& v) {
    value = v;
}

template <typename V>
void CacheEntry<V>::setTTL(time_t t) {
    if (t < 0)
        throw std::invalid_argument("CacheEntry: TTL cannot be negative");
    ttl = t;
}

template <typename V>
void CacheEntry<V>::touch() {
    if (frequency == std::numeric_limits<size_t>::max())
        throw std::overflow_error("CacheEntry: frequency counter overflow");
    timestamp = currentTime();
    frequency++;
}

template <typename V>
bool CacheEntry<V>::isExpired() const {
    if (ttl == 0)
        return false;
    return currentTime() > insertionTime + ttl;
}

template <typename V>
std::ostream& operator<<(std::ostream& os, const CacheEntry<V>& obj) {
    os << "Value: " << obj.value << "\n";
    os << "Timestamp: " << obj.timestamp << "\n";
    os << "Insertion Time: " << obj.insertionTime << "\n";
    os << "Frequency: " << obj.frequency << "\n";
    os << "TTL: " << obj.ttl << "\n";
    return os;
}