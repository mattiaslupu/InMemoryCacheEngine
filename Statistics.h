#pragma once
#include <iostream>
#include <string>

class Statistics {
private:
    size_t hits;
    size_t misses;
    size_t evictions;
    size_t insertions;

public:
    Statistics();
    Statistics(const Statistics& obj);

    Statistics& operator=(const Statistics& obj);

    ~Statistics();

    void recordHit();
    void recordMiss();
    void recordEviction();
    void recordInsertion();

    size_t getHits() const;
    size_t getMisses() const;
    size_t getEvictions() const;
    size_t getInsertions() const;
    size_t getTotalRequests() const;

    double getHitRate() const;

    void reset();

    friend std::ostream& operator<<(std::ostream& os, const Statistics& obj);
    friend std::istream& operator>>(std::istream& is, Statistics& obj);
};