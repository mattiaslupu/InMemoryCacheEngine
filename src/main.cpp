#include "Cache.h"
#include "Menu.h"
#include "PolicyFactory.h"
#include "Config.h"
#include "Demo.h"
#include <iostream>

int main() {
    try {
        Config config("data/config.txt");

        demoGenericCache();

        Cache<std::string, std::string> cache(
            config.getCapacity(),
            PolicyFactory<std::string>::create(config.getPolicy()));

        Menu menu(cache);
        menu.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
