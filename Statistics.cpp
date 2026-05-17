#include "Statistics.h"
#include <stdexcept>

Statistics::Statistics() : hits{0}, misses{0}, evictions{0}, insertions{0} {}

Statistics::Statistics(const Statistics& obj)
    : hits{obj.hits},
      misses{obj.misses},
      evictions{obj.evictions},
      insertions{obj.insertions} {}

Statistics& Statistics::operator=(const Statistics& obj) {
    if (this != &obj) {
        hits = obj.hits;
        misses = obj.misses;
        insertions = obj.insertions;
        evictions = obj.evictions;
    }
    return *this;
}

Statistics::~Statistics() {}

size_t Statistics::getHits() const {
    return hits;
}

size_t Statistics::getMisses() const {
    return misses;
}

size_t Statistics::getEvictions() const {
    return evictions;
}

size_t Statistics::getInsertions() const {
    return insertions;
}

size_t Statistics::getTotalRequests() const {
    return hits + misses;
}

double Statistics::getHitRate() const {
    if (hits + misses == 0)
        return 0.0;
    return static_cast<double>(hits) / (hits + misses) * 100;
}

void Statistics::recordHit() {
    hits++;
}

void Statistics::recordMiss() {
    misses++;
}

void Statistics::recordEviction() {
    evictions++;
}

void Statistics::recordInsertion() {
    insertions++;
}

void Statistics::reset() {
    hits = 0;
    misses = 0;
    evictions = 0;
    insertions = 0;
}



std::ostream& operator<<(std::ostream& os, const Statistics& obj) {
    os << "Hits:       " << obj.hits << "\n";
    os << "Misses:     " << obj.misses << "\n";
    os << "Evictions:  " << obj.evictions << "\n";
    os << "Insertions: " << obj.insertions << "\n";
    os << "Total:      " << obj.getTotalRequests() << "\n";
    os << "Hit Rate:   " << obj.getHitRate() << "%\n";
    return os;
}

std::istream& operator>>(std::istream& is, Statistics& obj) {
    size_t h, m, ev, ins;
    if (!(is >> h >> m >> ev >> ins))
        throw std::invalid_argument("Statistics: invalid input");
    obj.hits = h;
    obj.misses = m;
    obj.evictions = ev;
    obj.insertions = ins;
    return is;
}