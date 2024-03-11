#include "weather.h"

Weather::Weather(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open config file." << std::endl;
        status_ = ProgrammStatus::ConfigError;
        return;
    }
    nlohmann::json config;
    file >> config;
    file.close();

    if (!config["frequency"].is_number_integer()) {
        std::cerr << "Error: frequency is not an integer." << std::endl;
        status_ = ProgrammStatus::ConfigError;
        return;
    }
    if (!config["days"].is_number_integer()) {
        std::cerr << "Error: days is not an integer." << std::endl;
        status_ = ProgrammStatus::ConfigError;
        return;
    }
    frequency_ = config["frequency"];
    days_ = config["days"];

    if (!config["cities"].is_array()) {
        std::cerr << "Error: cities is not an array." << std::endl;
        status_ = ProgrammStatus::ConfigError;
        return;
    }
    if (config["cities"].empty()) {
        std::cerr << "Error: cities is empty." << std::endl;
        status_ = ProgrammStatus::ConfigError;
        return;
    }

    for (const auto& city : config["cities"]) {
        if (!city.is_string()) {
            std::cerr << "Error: city is not a string." << std::endl;
            status_ = ProgrammStatus::ConfigError;
            return;
        }
        auto city_info = GetCityInfo(city);
        if (city_info.empty()) {
            std::cerr << "Error: cannot get city information about " << city
                      << "." << std::endl;
            status_ = ProgrammStatus::ConfigError;
            return;
        }
        cities_.push_back(city_info);
    }
}

nlohmann::json Weather::GetCityInfo(const std::string& city) {
    // https://api-ninjas.com/api/city
    cpr::Response response = cpr::Get(
        cpr::Url{"https://api.api-ninjas.com/v1/city"},
        cpr::Parameters{{"name", city}},
        cpr::Header{{"X-Api-Key", "YOUR_API_KEY"}});
    if (response.status_code != 200) {
        return nlohmann::json();
    }
    nlohmann::json city_info = nlohmann::json::parse(response.text);
    if (city_info.empty()) {
        return nlohmann::json();
    }
    return city_info;
}

nlohmann::json Weather::GetWeatherInfo(double lat, double lon) {
    // https://api.open-meteo.com/v1/forecast?latitude=55.755800&longitude=37.617800&hourly=temperature_2m&forecast_days=1
    cpr::Response response =
        cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"},
                 cpr::Parameters{{"latitude", std::to_string(lat)},
                                 {"longitude", std::to_string(lon)},
                                 {"hourly",
                                  "temperature_2m,weathercode,wind_speed_10m,"
                                  "precipitation_probability,precipitation"},
                                 {"forecast_days", std::to_string(days_)},
                                 {"timezone", "auto"}});
    if (response.status_code != 200) {
        return nlohmann::json();
    }
    nlohmann::json weather_info = nlohmann::json::parse(response.text);
    if (weather_info.empty()) {
        return nlohmann::json();
    }
    return weather_info;
}

void Weather::Run() {
    if (status_ == ProgrammStatus::ConfigError) {
        std::cout << "Programm endend with config error." << std::endl;
        return;
    }
    std::thread keyboard([&]() {
        while (status_ == ProgrammStatus::Running) {
            if (GetAsyncKeyState(0x4E) & 0x8000) {
                current_city_ = (current_city_ + 1) % cities_.size();
                UpdateView();
            }
            if (GetAsyncKeyState(0x50) & 0x8000) {
                current_city_ =
                    (current_city_ - 1 + cities_.size()) % cities_.size();
                UpdateView();
            }
            if (GetAsyncKeyState(0xBB) & 0x8000) {
                days_ = std::min(days_ + 1, 16);
                UpdateView();
            }
            if (GetAsyncKeyState(0xBD) & 0x8000){
                days_ = std::max(days_ - 1, 1);
                UpdateView();
            }
            if (GetAsyncKeyState(0x1B) & 0x8000) {
                status_ = ProgrammStatus::Stopped;
                return;
            }
        }
    });

    std::atomic<bool> stop_view(false);
    std::condition_variable cv;
    std::mutex cv_m;
    std::thread view([&]() {
        std::unique_lock<std::mutex> lk(cv_m);
        while (status_ == ProgrammStatus::Running) {
            UpdateView();
            cv.wait_for(lk, std::chrono::seconds(frequency_));
            if (stop_view.load()) {
                return;
            }
        }
    });

    keyboard.join();
    stop_view.store(true);
    cv.notify_one();
    view.join();
    if (status_ == ProgrammStatus::WeatherError) {
        std::cout << "Programm endend with weather error." << std::endl;
        return;
    }
    system("CLS");
    std::cout << "\033[38;5;230;1mGoodbye!\033[0m" << std::endl;
    std::cout << "Programm endend with exit." << std::endl;
}
