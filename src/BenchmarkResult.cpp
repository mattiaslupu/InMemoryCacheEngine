#include "BenchmarkResult.h"
#include <stdexcept>

BenchmarkResult::BenchmarkResult()
    : policyName{""}, hitRate{0.0}, hits{0},
      misses{0}, evictions{0}, durationMs{0} {}

BenchmarkResult::BenchmarkResult(const std::string& policyName, double hitRate, size_t hits, size_t misses, size_t evictions, long long durationMs) : policyName{policyName}, hitRate{hitRate}, hits{hits}, misses{misses}, evictions{evictions}, durationMs{durationMs} {
    if (policyName.empty())
        throw std::invalid_argument("BenchmarkResult: policy name cannot be empty");
    if (hitRate < 0.0 || hitRate > 100.0)
        throw std::invalid_argument("BenchmarkResult: hit rate must be between 0 and 100");
}

std::string BenchmarkResult::getPolicyName() const {
    return policyName;
}

double BenchmarkResult::getHitRate() const {
    return hitRate;
}
size_t BenchmarkResult::getHits() const {
    return hits;
}

size_t BenchmarkResult::getMisses() const {
    return misses;
}

size_t BenchmarkResult::getEvictions() const {
    return evictions;
}


long long BenchmarkResult::getDurationMs() const {
    return durationMs;
}

std::ostream& operator<<(std::ostream& os, const BenchmarkResult& obj) {
    os << "Policy: " << obj.policyName << "\n";
    os << "Hit Rate: " << obj.hitRate << "%\n";
    os << "Hits: " << obj.hits << "\n";
    os << "Misses: " << obj.misses << "\n";
    os << "Evictions: " << obj.evictions << "\n";
    os << "Duration: " << obj.durationMs << "ms\n";
    return os;
}

std::istream& operator>>(std::istream& is, BenchmarkResult& obj) {
    std::string name;
    double hitRate;
    size_t hits, misses, evictions;
    long long durationMs;

    if (!(is >> name))
        throw std::invalid_argument("BenchmarkResult: missing policy name");
    if (name.empty())
        throw std::invalid_argument("BenchmarkResult: policy name cannot be empty");

    if (!(is >> hitRate))
        throw std::invalid_argument("BenchmarkResult: missing hit rate");
    if (hitRate < 0.0 || hitRate > 100.0)
        throw std::invalid_argument("BenchmarkResult: hit rate must be between 0 and 100");

    if (!(is >> hits))
        throw std::invalid_argument("BenchmarkResult: missing hits");
    if (!(is >> misses))
        throw std::invalid_argument("BenchmarkResult: missing misses");
    if (!(is >> evictions))
        throw std::invalid_argument("BenchmarkResult: missing evictions");
    if (!(is >> durationMs))
        throw std::invalid_argument("BenchmarkResult: missing duration");
    if (durationMs < 0)
        throw std::invalid_argument("BenchmarkResult: duration cannot be negative");

    obj.policyName = name;
    obj.hitRate = hitRate;
    obj.hits = hits;
    obj.misses = misses;
    obj.evictions = evictions;
    obj.durationMs = durationMs;

    return is;
}