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
    std::shared_ptr<Cache<std::string, std::string>> activeCache;
    std::string activeName;
    bool running;
    std::vector<std::shared_ptr<ICache>> managedCaches;

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
    void handleSwitchActiveCache();

    int readIntInRange(int min, int max) const;
    std::string readNonEmptyString(const std::string& prompt) const;

public:
    Menu(std::shared_ptr<Cache<std::string, std::string>> cache, const std::string& name);
    ~Menu();

    Menu(const Menu&)            = delete;
    Menu& operator=(const Menu&) = delete;
    Menu(Menu&&)                 = delete;
    Menu& operator=(Menu&&)      = delete;

    void run();
};
