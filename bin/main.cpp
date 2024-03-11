#include <cstdint>
#include <iostream>

#include "lib/weather.h"

int main(int argc, char** argv) {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    if (argc < 2) {
        std::cerr << "Error: no config file." << std::endl;
        return 1;
    }
    Weather weather(argv[1]);
    weather.Run();
    return 0;
}
