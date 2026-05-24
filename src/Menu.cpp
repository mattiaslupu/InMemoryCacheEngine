#include "Menu.h"
#include "CacheException.h"
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>

Menu::Menu(Cache<std::string, std::string>& cache)
    : cache{cache}, running{true} {}

Menu::~Menu() {}

void Menu::run() {
    while (running) {
        displayMainMenu();
        int choice = readIntInRange(0, 11);
        switch (choice) {
            case  0: running = false;         break;
            case  1: handlePut();             break;
            case  2: handleGet();             break;
            case  3: handleDelete();          break;
            case  4: handleDisplayContents(); break;
            case  5: handleSwitchPolicy();    break;
            case  6: handleResize();          break;
            case  7: handleStatistics();      break;
            case  8: handleLoadFromFile();    break;
            case  9: handleBenchmark();       break;
            case 10: handleCacheManager();    break;
            case 11: handleReset();           break;
        }
    }
    std::cout << "\nGoodbye!\n";
}

void Menu::displayHeader() const {
    std::cout << "\n";
    std::cout << "===========================================\n";
    std::cout << "          In-Memory Cache Engine\n";
    std::cout << "===========================================\n";
    std::cout << "  Policy  : " << cache.getPolicyName() << "\n";
    std::cout << "  Entries : " << cache.size() << " / " << cache.getCapacity() << "\n";
    std::cout << "===========================================\n";
}

void Menu::displayMainMenu() const {
    displayHeader();
    std::cout << "  [ 1] Put\n";
    std::cout << "  [ 2] Get\n";
    std::cout << "  [ 3] Delete\n";
    std::cout << "  [ 4] Display Contents\n";
    std::cout << "  [ 5] Switch Policy\n";
    std::cout << "  [ 6] Resize\n";
    std::cout << "  [ 7] Statistics\n";
    std::cout << "  [ 8] Load From File\n";
    std::cout << "  [ 9] Benchmark\n";
    std::cout << "  [10] Cache Manager\n";
    std::cout << "  [11] Reset\n";
    std::cout << "  [ 0] Exit\n";
    std::cout << "===========================================\n";
}

void Menu::handlePut() {
    std::cout << "\n-- Put --\n";
    try {
        std::string key   = readNonEmptyString("Key   : ");
        std::string value = readNonEmptyString("Value : ");
        cache.put(key, value);
        std::cout << "Stored [" << key << "] -> \"" << value << "\"\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleGet() {
    std::cout << "\n-- Get --\n";
    try {
        std::string key = readNonEmptyString("Key: ");
        std::string* result = cache.get(key);
        if (result)
            std::cout << "[" << key << "] -> \"" << *result << "\"\n";
        else
            std::cout << "Key not found: \"" << key << "\"\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleDelete() {
    std::cout << "\n-- Delete --\n";
    try {
        std::string key = readNonEmptyString("Key: ");
        if (cache.remove(key))
            std::cout << "Deleted [" << key << "]\n";
        else
            std::cout << "Key not found: \"" << key << "\"\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleDisplayContents() {
    std::cout << "\n-- Cache Contents --\n";
    try {
        std::cout << cache;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleSwitchPolicy() {
    std::cout << "\n-- Switch Policy --\n";
    std::cout << "Current policy: " << cache.getPolicyName() << "\n\n";

    std::vector<std::string> policies = PolicyFactory<std::string>::availablePolicies();
    for (int i = 0; i < static_cast<int>(policies.size()); ++i)
        std::cout << "  [" << (i + 1) << "] " << policies[i] << "\n";
    std::cout << "  [0] Back\n";

    try {
        int choice = readIntInRange(0, static_cast<int>(policies.size()));
        if (choice == 0) return;
        const std::string& chosen = policies[choice - 1];
        std::unique_ptr<EvictionPolicy<std::string>> newPolicy = PolicyFactory<std::string>::create(chosen);
        cache.switchPolicy(std::move(newPolicy));
        std::cout << "Policy switched to: " << cache.getPolicyName() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleResize() {
    std::cout << "\n-- Resize --\n";
    std::cout << "Current capacity: " << cache.getCapacity() << "\n";
    std::cout << "New capacity: ";
    try {
        int newCap = readIntInRange(1, 1000000);
        cache.resize(static_cast<size_t>(newCap));
        std::cout << "Capacity set to " << cache.getCapacity() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleStatistics() {
    std::cout << "\n-- Statistics --\n";
    std::cout << cache.getStats();
}

void Menu::handleLoadFromFile() {
    std::cout << "\n-- Load From File --\n";
    std::cout << "Format: each line must be \"key value\"\n";
    try {
        std::string filename = readNonEmptyString("Filename: ");
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file: " + filename);

        int loaded = 0;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string key, value;
            if (!(ss >> key)) continue;
            std::getline(ss, value);
            if (!value.empty() && value[0] == ' ')
                value = value.substr(1);
            cache.put(key, value);
            ++loaded;
        }
        std::cout << "Loaded " << loaded << " entries from \"" << filename << "\"\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleBenchmark() {
    std::cout << "\n-- Benchmark --\n";
    std::cout << "  [1] Random workload on current cache\n";
    std::cout << "  [2] Random workload — compare all policies\n";
    std::cout << "  [3] Load workload from file, run on current cache\n";
    std::cout << "  [4] Load workload from file — compare all policies\n";
    std::cout << "  [0] Back\n";

    try {
        int choice = readIntInRange(0, 4);
        if (choice == 0) return;
        Benchmark bench(cache.getCapacity());

        if (choice == 1 || choice == 2) {
            std::cout << "Number of operations: ";
            int ops = readIntInRange(1, 1000000);
            std::cout << "Key space size: ";
            int keySpace = readIntInRange(1, 100000);
            bench.generateRandomWorkload(static_cast<size_t>(ops), static_cast<size_t>(keySpace));
            std::cout << "Generated " << bench.getOperationCount() << " operations.\n";
        } else {
            std::string filename = readNonEmptyString("Workload file: ");
            bench.loadWorkload(filename);
            std::cout << "Loaded " << bench.getOperationCount() << " operations.\n";
        }

        if (choice == 1 || choice == 3) {
            std::cout << "Warning: the current cache will be cleared during the run.\n";
            std::cout << "Continue? [1] Yes  [2] No\n";
            if (readIntInRange(1, 2) != 1) return;
            BenchmarkResult result = bench.run(cache);
            std::cout << result;
        } else {
            bench.compareAllPolicies();
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Menu::handleCacheManager() {
    std::cout << "\n-- Cache Manager (Singleton) --\n";

    bool inSubmenu = true;
    while (inSubmenu) {
        std::cout << "\n  [1] Show all registered caches\n";
        std::cout << "  [2] Register a new named cache\n";
        std::cout << "  [3] Look up a registered cache\n";
        std::cout << "  [4] Remove a registered cache\n";
        std::cout << "  [0] Back\n";

        CacheManager& manager = CacheManager::getInstance();
        int choice = readIntInRange(0, 4);
        try {
            switch (choice) {
                case 1:
                    std::cout << manager;
                    break;

                case 2: {
                    std::string name = readNonEmptyString("Cache name: ");
                    std::cout << "Capacity: ";
                    int cap = readIntInRange(1, 1000000);

                    std::vector<std::string> policies = PolicyFactory<std::string>::availablePolicies();
                    for (int i = 0; i < static_cast<int>(policies.size()); ++i)
                        std::cout << "  [" << (i + 1) << "] " << policies[i] << "\n";
                    int pChoice = readIntInRange(1, static_cast<int>(policies.size()));
                    const std::string& policyName = policies[pChoice - 1];

                    auto newCache = std::make_shared<Cache<std::string, std::string>>(
                        static_cast<size_t>(cap), PolicyFactory<std::string>::create(policyName));

                    manager.registerCache(name, newCache);
                    managedCaches.push_back(newCache);

                    std::cout << "Registered cache \"" << name
                              << "\" (policy=" << policyName
                              << ", capacity=" << cap << ")\n";
                    break;
                }

                case 3: {
                    std::string name = readNonEmptyString("Cache name: ");
                    auto found = manager.getCache(name);
                    std::cout << "=== " << name << " ===\n" << *found;
                    break;
                }

                case 4: {
                    std::string name = readNonEmptyString("Cache name: ");
                    manager.removeCache(name);
                    std::cout << "Removed \"" << name << "\" from the registry.\n";
                    break;
                }

                case 0:
                    inSubmenu = false;
                    break;
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void Menu::handleReset() {
    std::cout << "\n-- Reset --\n";
    try {
        cache.clear();
        std::cout << "Cache cleared.\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

int Menu::readIntInRange(int min, int max) const {
    int value;
    while (true) {
        std::cout << "Choice [" << min << "-" << max << "]: ";
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number between "
                  << min << " and " << max << ".\n";
    }
}

std::string Menu::readNonEmptyString(const std::string& prompt) const {
    std::string s;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, s);
        if (!s.empty())
            return s;
        std::cout << "Input cannot be empty. Try again.\n";
    }
}
