cmake_minimum_required(VERSION 3.14)

project(EVC)


add_executable(main_runner evc.cpp token.cpp scanner.cpp)

add_executable(tests test.cpp)