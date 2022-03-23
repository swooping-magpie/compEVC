#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdio.h>

int main(int argc, char **argv) {
  std::printf("======= The VC compiler =======\n");

  std::ifstream src_file;

  src_file.open("../test.vc");
  assert(src_file.is_open());

  // character iterator over a stream

  for (auto i = 0; i < 10; ++i) {
    char c = src_file.get();
    std::printf("new char is: %c\n", c);
  }
}