#pragma once
#include <string>
#include <iostream>

class BenchmarkResult {
private:
    std::string policyName;
    double hitRate;
    size_t hits;
    size_t misses;
    size_t evictions;
    long long durationMs;

public:
    BenchmarkResult();
    BenchmarkResult(const std::string& policyName, double hitRate, size_t hits, size_t misses, size_t evictions, long long durationMs);
    BenchmarkResult(const BenchmarkResult& obj);
    BenchmarkResult(BenchmarkResult&& obj) noexcept;
    BenchmarkResult& operator=(const BenchmarkResult& obj);
    BenchmarkResult& operator=(BenchmarkResult&& obj) noexcept;
    ~BenchmarkResult();

    std::string getPolicyName() const;
    double getHitRate() const;
    size_t getHits() const;
    size_t getMisses() const;
    size_t getEvictions() const;
    long long getDurationMs() const;

    friend std::ostream& operator<<(std::ostream& os, const BenchmarkResult& obj);
    friend std::istream& operator>>(std::istream& is, BenchmarkResult& obj);
};