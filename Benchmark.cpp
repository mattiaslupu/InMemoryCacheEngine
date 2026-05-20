#include "Benchmark.h"
#include <stdexcept>

Benchmark::Benchmark()
    : operations{},
      capacity{10} {}

Benchmark::Benchmark(size_t capacity)
    : operations{},
      capacity{capacity} {
    if (capacity == 0)
        throw std::invalid_argument("Benchmark: capacity must be greater than 0");
}


void Benchmark::loadWorkload(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Critical error: Cannot open file " + filename);
    }
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        Operation op;
        ss >> op;
        operations.push_back(op);

    }

}


void Benchmark::generateRandomWorkload(size_t count, size_t keySpace) {
    if (count == 0)
        throw std::invalid_argument("Benchmark: count must be greater than 0");
    if (keySpace == 0)
        throw std::invalid_argument("Benchmark: keySpace must be greater than 0");

    operations.clear();
    for (size_t i = 0; i < count; i++) {
        int random = rand() % 3;
        std::string key = "key:" + std::to_string(rand() % keySpace);

        if (random == 0) {
            std::string value = "value:" + std::to_string(rand() % 100);
            operations.push_back(Operation(OpType::PUT, key, value));
        } else if (random == 1) {
            operations.push_back(Operation(OpType::GET, key));
        } else {
            operations.push_back(Operation(OpType::REMOVE, key));
        }
    }
}

BenchmarkResult Benchmark::run(Cache<std::string, std::string>& cache) {
    cache.clear();

    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& op : operations) {
        if (op.getType() == OpType::PUT)
            cache.put(op.getKey(), op.getValue());
        else if (op.getType() == OpType::GET)
            cache.get(op.getKey());
        else if (op.getType() == OpType::REMOVE)
            cache.remove(op.getKey());
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return BenchmarkResult(cache.getPolicyName(), cache.getStats().getHitRate(), cache.getStats().getHits(), cache.getStats().getMisses(), cache.getStats().getEvictions(), ms);
}

void Benchmark::printComparisonTable(const std::vector<BenchmarkResult>& results) const {
    const int col1 = 10;
    const int col2 = 11;
    const int col3 = 7;
    const int col4 = 8;
    const int col5 = 11;
    const int col6 = 10;

    auto line = [&]() {
        std::cout << "+" << std::string(col1, '-')
                  << "+" << std::string(col2, '-')
                  << "+" << std::string(col3, '-')
                  << "+" << std::string(col4, '-')
                  << "+" << std::string(col5, '-')
                  << "+" << std::string(col6, '-')
                  << "+\n";
    };

    line();
    std::cout << "| " << std::left << std::setw(col1 - 1) << "Policy"
              << "| " << std::setw(col2 - 1) << "Hit Rate"
              << "| " << std::setw(col3 - 1) << "Hits"
              << "| " << std::setw(col4 - 1) << "Misses"
              << "| " << std::setw(col5 - 1) << "Evictions"
              << "| " << std::setw(col6 - 1) << "Duration"
              << "|\n";
    line();

    for (const auto& result : results) {
        std::cout << "| " << std::left << std::setw(col1 - 1) << result.getPolicyName()
                  << "| " << std::setw(col2 - 1) << (std::to_string(result.getHitRate()).substr(0, 5) + "%")
                  << "| " << std::setw(col3 - 1) << result.getHits()
                  << "| " << std::setw(col4 - 1) << result.getMisses()
                  << "| " << std::setw(col5 - 1) << result.getEvictions()
                  << "| " << std::setw(col6 - 1) << (std::to_string(result.getDurationMs()) + "ms")
                  << "|\n";
    }

    line();
}

void Benchmark::compareAllPolicies() {
    if (operations.empty())
        throw std::runtime_error("Benchmark: no operations loaded");

    std::vector<BenchmarkResult> results;

    for (const auto& policyName : PolicyFactory::availablePolicies()) {
        Cache<std::string, std::string> cache(capacity, PolicyFactory::create(policyName));
        BenchmarkResult result = run(cache);
        results.push_back(result);
    }

    printComparisonTable(results);
}
void Benchmark::setCapacity(size_t cap) {
    if (cap == 0)
        throw std::invalid_argument("Benchmark: capacity must be greater than 0");
    capacity = cap;
}

size_t Benchmark::getCapacity() const {
    return capacity;
}

size_t Benchmark::getOperationCount() const {
    return operations.size();
}

std::ostream& operator<<(std::ostream& os, const Benchmark& obj) {
    os << "Capacity: " << obj.capacity << "\n";
    os << "Operations: " << obj.operations.size() << "\n";
    for (const auto& op : obj.operations)
        os << "  " << op;
    return os;
}

std::istream& operator>>(std::istream& is, Benchmark& obj) {
    size_t cap;
    if (!(is >> cap))
        throw std::invalid_argument("Benchmark: invalid capacity input");
    if (cap == 0)
        throw std::invalid_argument("Benchmark: capacity must be greater than 0");
    obj.capacity = cap;

    size_t count;
    if (!(is >> count))
        throw std::invalid_argument("Benchmark: invalid operation count");

    obj.operations.clear();
    for (size_t i = 0; i < count; i++) {
        Operation op;
        is >> op;
        obj.operations.push_back(op);
    }
    return is;
}