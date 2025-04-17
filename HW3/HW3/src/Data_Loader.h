#ifndef DATA_LOADER_H
#define DATA_LOADER_H

using namespace std;
#include <string>
#include "Data_Structure.h"
ifstream read_file(string filename);
void build_data_structure(Info &info, ifstream &input);
void write_output(Info& info, string filename);

#endif