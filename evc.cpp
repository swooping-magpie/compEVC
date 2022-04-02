#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "scanner.hpp"

int main(int argc, char **argv) {
  std::printf("======= The VC compiler =======\n");

  std::ifstream src_file("../test.vc");
  assert(src_file.is_open());

  std::stringstream filebuf;
  filebuf << src_file.rdbuf();

  auto f = filebuf.str();
}