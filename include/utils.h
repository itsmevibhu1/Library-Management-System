#pragma once
#include <string>
#include <ctime>

int getValidIntegerInput(const std::string& prompt);
time_t calculatetime(time_t curr_time, int days);