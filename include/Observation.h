#pragma once

#include <string>

struct Observation
{
    int id = 0;
    int stationId = 0;
    std::string observedAt;
    double temperatureC = 0.0;
    double humidityPercent = 0.0;
    double pressureHpa = 0.0;
    double windSpeedMs = 0.0;
    double precipitationMm = 0.0;
};
