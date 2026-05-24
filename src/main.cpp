#include "Cache.h"
#include "Menu.h"
#include "PolicyFactory.h"
#include "TTLPolicy.h"
#include "Config.h"
#include "Demo.h"
#include <iostream>

int main() {
    try {
        Config config("data/config.txt");

        demoGenericCache();

        auto policy = PolicyFactory<std::string>::create(config.getPolicy());
        if (auto* ttlPolicy = dynamic_cast<TTLPolicy<std::string>*>(policy.get()))
            ttlPolicy->setDefaultTTL(static_cast<time_t>(config.getDefaultTTL()));

        Cache<std::string, std::string> cache(config.getCapacity(), std::move(policy));

        Menu menu(cache);
        menu.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
