#pragma once
#include <string>

std::string get_executable_fname();
std::string get_executable_dir();
std::string get_ro_data_dir();
std::string get_rw_data_dir();

bool exists(std::string &path);
bool is_dir(std::string &path);
