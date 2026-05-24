#pragma once
#include "Cache.h"
#include "CacheManager.h"
#include "Benchmark.h"
#include "PolicyFactory.h"
#include <string>
#include <iostream>
#include <vector>
#include <memory>

class Menu {
private:
    Cache<std::string, std::string>& cache;
    bool running;
    std::vector<std::shared_ptr<Cache<std::string, std::string>>> managedCaches;

    void displayMainMenu() const;
    void displayHeader() const;

    void handlePut();
    void handleGet();
    void handleDelete();
    void handleDisplayContents();
    void handleSwitchPolicy();
    void handleResize();
    void handleStatistics();
    void handleLoadFromFile();
    void handleBenchmark();
    void handleReset();
    void handleCacheManager();

    int readIntInRange(int min, int max) const;
    std::string readNonEmptyString(const std::string& prompt) const;

public:
    explicit Menu(Cache<std::string, std::string>& cache);
    ~Menu();

    Menu(const Menu&)            = delete;
    Menu& operator=(const Menu&) = delete;
    Menu(Menu&&)                 = delete;
    Menu& operator=(Menu&&)      = delete;

    void run();
};
