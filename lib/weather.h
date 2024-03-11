#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"

class Weather {
   public:
    Weather(const std::string& config_file);

    void Run();

   private:
    enum ProgrammStatus { Running, Stopped, ConfigError, WeatherError };

    nlohmann::json GetCityInfo(const std::string& city);
    nlohmann::json GetWeatherInfo(double latitude, double longitude);

    void UpdateView();
    std::string GetWeatherType(int32_t code);

    std::vector<nlohmann::json> cities_;
    int64_t frequency_;
    int32_t days_;

    int64_t current_city_ = 0;
    ProgrammStatus status_ = ProgrammStatus::Running;
};
