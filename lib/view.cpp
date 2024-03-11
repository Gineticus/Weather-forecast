#include "weather.h"

void Weather::UpdateView() {
    auto city = cities_[current_city_];
    auto weather = GetWeatherInfo(city[0]["latitude"], city[0]["longitude"]);
    system("CLS");
    if (weather.empty()) {
        std::cerr << "Error: cannot get weather information" << std::endl;
        status_ = ProgrammStatus::WeatherError;
        return;
    }

    std::stringstream output;
    output << "\033[38;5;226m     __    __           _   _\033[0m" << '\n';
    output << "\033[38;5;226m    / / /\\ \\ \\___  __ _| |_| |__   ___ _ __ \033[0m" << '\n';
    output << "\033[38;5;226m    \\ \\/  \\/ / _ \\/ _` | __| '_ \\ / _ \\ '__|\033[0m" << '\n';
    output << "\033[38;5;226m     \\  /\\  /  __/ (_| | |_| | | |  __/ |\033[0m" << '\n';
    output << "\033[38;5;226m      \\/  \\/ \\___|\\__,_|\\__|_| |_|\\___|_| \033[0m"
           << "\033[38;5;254;1m in " << city[0]["name"].get<std::string>() << "\033[0m" << '\n';
    output << "\033[38;5;253m";
    for (int64_t day = 0; day < days_; ++day) {
        output << "                                                         ┌─────────────┐"<< '\n';
        output << "  ┌──────────────────────────────┬───────────────────────┤ " + weather["hourly"]["time"][24 * day].get<std::string>().substr(0, 10) +"  ├───────────────────────┬──────────────────────────────┐" << '\n';
        output << "  │           Morning            │          Afternoon    └──────┬──────┘    Evening            │            Night             │" << '\n';
        output << "  ├──────────────────────────────┼──────────────────────────────┼──────────────────────────────┼──────────────────────────────┤" << '\n';
        output << "  │ Weather: " << std::setw(20) << std::left << 
        (weather["hourly"]["weathercode"][5 + 24 * day].is_number_integer() ? GetWeatherType(weather["hourly"]["weathercode"][5 + 24 * day]) : "Unknown") <<
        "│ Weather: " << std::setw(20) << std::left <<
        (weather["hourly"]["weathercode"][11 + 24 * day].is_number_integer() ? GetWeatherType(weather["hourly"]["weathercode"][11 + 24 * day]) : "Unknown") <<
        "│ Weather: " << std::setw(20) << std::left <<
        (weather["hourly"]["weathercode"][17 + 24 * day].is_number_integer() ? GetWeatherType(weather["hourly"]["weathercode"][17 + 24 * day]) : "Unknown") <<
        "│ Weather: " << std::setw(20) << std::left <<
        (weather["hourly"]["weathercode"][23 + 24 * day].is_number_integer() ? GetWeatherType(weather["hourly"]["weathercode"][23 + 24 * day]) : "Unknown") <<
        "│" << '\n';
        output << "  │ Temperature: " << std::setw(17) << std::left << to_string(weather["hourly"]["temperature_2m"][5 + 24 * day]) + "°C" <<
        "│ Temperature: " << std::setw(17) << std::left << to_string(weather["hourly"]["temperature_2m"][11 + 24 * day]) + "°C" <<
        "│ Temperature: " << std::setw(17) << std::left << to_string(weather["hourly"]["temperature_2m"][17 + 24 * day]) + "°C" <<
        "│ Temperature: " << std::setw(17) << std::left << to_string(weather["hourly"]["temperature_2m"][23 + 24 * day]) + "°C" <<
        "│" << '\n';
        output << "  │ Wind speed: " << std::setw(17) << std::left << to_string(weather["hourly"]["wind_speed_10m"][5 + 24 * day]) + "m/s" <<
        "│ Wind speed: " << std::setw(17) << std::left << to_string(weather["hourly"]["wind_speed_10m"][11 + 24 * day]) + "m/s" <<
        "│ Wind speed: " << std::setw(17) << std::left << to_string(weather["hourly"]["wind_speed_10m"][17 + 24 * day]) + "m/s" <<
        "│ Wind speed: " << std::setw(17) << std::left << to_string(weather["hourly"]["wind_speed_10m"][23 + 24 * day]) + "m/s" <<
        "│" << '\n';
        output << "  │ Precipitation: " << std::setw(14) << std::left << 
        to_string(weather["hourly"]["precipitation"][5 + 24 * day]) + "mm|" + to_string(weather["hourly"]["precipitation_probability"][5 + 24 * day])+"%"<<
        "│ Precipitation: " << std::setw(14) << std::left << 
        to_string(weather["hourly"]["precipitation"][11 + 24 * day]) + "mm|" + to_string(weather["hourly"]["precipitation_probability"][11 + 24 * day])+"%"<<
        "│ Precipitation: " << std::setw(14) << std::left << 
        to_string(weather["hourly"]["precipitation"][17 + 24 * day]) + "mm|" + to_string(weather["hourly"]["precipitation_probability"][17 + 24 * day])+"%"<<
        "│ Precipitation: " << std::setw(14) << std::left << 
        to_string(weather["hourly"]["precipitation"][23 + 24 * day]) + "mm|" + to_string(weather["hourly"]["precipitation_probability"][23 + 24 * day])+"%"<<
        "│" << '\n';
        output << "  └──────────────────────────────┴──────────────────────────────┴──────────────────────────────┴──────────────────────────────┘" << '\n';
    }
    output << "\033[0m";
    std::cout << output.str();
}

std::string Weather::GetWeatherType(int32_t code) {
    if (code == 0) {
        return "Clear sky";
    } else if (1 <= code && code <= 3) {
        return "Cloudy";
    } else if (44 < code && code < 49) {
        return "Fog";
    } else if (50 < code && code < 58) {
        return "Drizzle";
    } else if (60 < code && code < 66) {
        return "Rain";
    } else if (70 < code && code < 78) {
        return "Snow";
    } else if (79 < code && code < 83) {
        return "Rain showers";
    } else if (84 < code && code < 87) {
        return "Snow showers";
    } else if (90 < code && code < 100) {
        return "Thunderstorm";
    } else {
        return "Unknown";
    }
}