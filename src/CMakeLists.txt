cmake_minimum_required(VERSION 3.10)
project(journal_lib LANGUAGES CXX)

# option for static or shared library
option(BUILD_SHARED_LIB "build journal as a shared library" OFF)

# create library
if(BUILD_SHARED_LIB)
    add_library(journal SHARED myJournal.cpp)
else()
    add_library(journal STATIC myJournal.cpp)
endif()

target_include_directories(journal PUBLIC ../include)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
install(TARGETS journal DESTINATION lib)
