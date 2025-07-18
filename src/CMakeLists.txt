cmake_minimum_required(VERSION 3.10)
project(journal_lib LANGUAGES CXX)

add_library(journal STATIC myJournal.cpp)
target_include_directories(journal PUBLIC ../include)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
install(TARGETS journal DESTINATION lib)
