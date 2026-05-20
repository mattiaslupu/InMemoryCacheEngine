#include "EvictionPolicy.h"

EvictionPolicy::EvictionPolicy() : evictionCount(0) {

}


size_t EvictionPolicy::getEvictionCount() const {
    return evictionCount;
}

std::ostream &operator<<(std::ostream &os, const EvictionPolicy &obj) {
    os<<"Eviction Count: "<<obj.evictionCount<<"\n";
    return os;
}
