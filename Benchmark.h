#pragma once
#include "Cache.h"
#include "PolicyFactory.h"
#include "Operation.h"
#include "BenchmarkResult.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <iomanip>

class Benchmark {
private:
    std::vector<Operation> operations;
    size_t capacity;

    void printComparisonTable(const std::vector<BenchmarkResult>& results) const;

public:
    Benchmark();
    Benchmark(size_t capacity);
    Benchmark(const Benchmark& obj) = default;
    Benchmark(Benchmark&& obj) noexcept = default;
    Benchmark& operator=(const Benchmark& obj) = default;
    Benchmark& operator=(Benchmark&& obj) noexcept = default;
    ~Benchmark() = default;

    void loadWorkload(const std::string& filename);
    void generateRandomWorkload(size_t count, size_t keySpace);
    BenchmarkResult run(Cache<std::string, std::string>& cache);
    void compareAllPolicies();

    void setCapacity(size_t cap);
    size_t getCapacity() const;
    size_t getOperationCount() const;

    friend std::ostream& operator<<(std::ostream& os, const Benchmark& obj);
    friend std::istream& operator>>(std::istream& is, Benchmark& obj);
};