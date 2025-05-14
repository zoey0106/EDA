#include <random>
#include <chrono>
using namespace std;

mt19937 gen(43); // origianl: 43
chrono::high_resolution_clock::time_point program_start = chrono::high_resolution_clock::now();