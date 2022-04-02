#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

#include "token.hpp"

std::vector<Token> do_scan(char const *start, uint32_t length);