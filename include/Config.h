#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class Config {
private:
    size_t capacity;
    long long defaultTTL;
    std::string policy;

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end   = s.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }

public:
    explicit Config(const std::string& filename) : capacity{100}, defaultTTL{60}, policy{"LRU"} {
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Config: cannot open file: " + filename);

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key   = trim(line.substr(0, eq));
            std::string value = trim(line.substr(eq + 1));
            if (key == "capacity")    capacity   = static_cast<size_t>(std::stoul(value));
            else if (key == "default_ttl") defaultTTL = std::stoll(value);
            else if (key == "policy") policy     = value;
        }
    }

    size_t      getCapacity()   const { return capacity; }
    long long   getDefaultTTL() const { return defaultTTL; }
    std::string getPolicy()     const { return policy; }
};
