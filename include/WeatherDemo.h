#pragma once

#include <string>

void prepareWeatherTestData(const std::string& databaseFile);
void runRepositoryDemo(const std::string& databaseFile);
void runQueryDemo(const std::string& databaseFile);
void runAggregateDemo(const std::string& databaseFile);
void runJoinDemo(const std::string& databaseFile);
void runTransactionDemo(const std::string& databaseFile);
void runRollbackDemo(const std::string& databaseFile);
void runWithoutTransactionDemo(const std::string& databaseFile);
