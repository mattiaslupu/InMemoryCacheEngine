#pragma once
#include <string>
#include <iostream>

class ICache {
public:
    virtual ~ICache() = default;

    virtual size_t size() const = 0;
    virtual size_t getCapacity() const = 0;
    virtual std::string getPolicyName() const = 0;
    virtual void print(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const ICache& c) {
        c.print(os);
        return os;
    }
};
