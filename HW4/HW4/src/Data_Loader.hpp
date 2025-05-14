#ifndef DATA_LOADER_H
#define DATA_LOADER_H

using namespace std;
#pragma once
#include <string>
#include "DataTypes.hpp"
ifstream read_file(string filename);
void build_data_structure(Info &info, ifstream &input);
void write_output(Info& data, string filename);

#endif