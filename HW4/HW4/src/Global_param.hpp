#ifndef GLOBAL_PARAM_H
#define GLOBAL_PARAM_H

using namespace std;
#include <string>
#include <random>
#include <chrono>
extern mt19937 gen;
extern chrono::high_resolution_clock::time_point program_start;
extern constexpr double TIME_LIMIT = 590.0;
#endif