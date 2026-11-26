PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS WeatherStation (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    region TEXT NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    elevationM REAL NOT NULL,
    status TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Observation (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    stationId INTEGER NOT NULL,
    observedAt TEXT NOT NULL,
    temperatureC REAL NOT NULL,
    humidityPercent REAL NOT NULL,
    pressureHpa REAL NOT NULL,
    windSpeedMs REAL NOT NULL,
    precipitationMm REAL NOT NULL,
    FOREIGN KEY (stationId) REFERENCES WeatherStation(id)
);
