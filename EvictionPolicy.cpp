#include "EvictionPolicy.h"

EvictionPolicy::EvictionPolicy() : evictionCount(0) {

}

EvictionPolicy::EvictionPolicy(const EvictionPolicy &obj) : evictionCount(obj.evictionCount){

}

size_t EvictionPolicy::getEvictionCount() const {
    return evictionCount;
}

EvictionPolicy &EvictionPolicy::operator=(const EvictionPolicy &obj) {
    if (this != & obj) {
        evictionCount=obj.evictionCount;
    }
    return *this;
}

EvictionPolicy::~EvictionPolicy() {}

std::ostream &operator<<(std::ostream &os, const EvictionPolicy &obj) {
    os<<"Eviction Count: "<<obj.evictionCount<<"\n";
    return os;
}
