#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

#include "scanner.hpp"
#include "token.hpp"

int main(int argc, char **argv) {

  assert(argc == 2 && "Please give us two args");

  std::printf("======= The VC compiler =======\n");

  std::ifstream src_file(argv[1]);
  assert(src_file.is_open());

  std::stringstream filebuf;
  filebuf << src_file.rdbuf();

  auto f = filebuf.str();

  std::vector<Token> tokens = do_scan(f.data(), f.size());

  for (auto const t : tokens) {
    std::printf("%s\n", to_string(t, f.data(), f.size()).c_str());
  }
}