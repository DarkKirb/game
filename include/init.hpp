#pragma once
#include <vector>
#include <string>

void init(int argc, const char **argv);
bool processEvents();
void swapFB();
void deinit();
std::vector<std::string> &getArgs();
