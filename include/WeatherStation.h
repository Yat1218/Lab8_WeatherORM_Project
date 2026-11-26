#pragma once

#include <string>

struct WeatherStation
{
    int id = 0;
    std::string name;
    std::string region;
    double latitude = 0.0;
    double longitude = 0.0;
    double elevationM = 0.0;
    std::string status;
};
