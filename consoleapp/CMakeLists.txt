cmake_minimum_required(VERSION 3.14)
project(JournalApp LANGUAGES CXX)

# set c++ standard to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find journal library (static or shared)
find_library(JOURNAL_LIB journal HINTS "${CMAKE_SOURCE_DIR}/../src/build/lib")
if(NOT JOURNAL_LIB)
    message(FATAL_ERROR "journal library not found")
endif()

# path to include
set(JOURNAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../include")

# create consoleapp executable
add_executable(consoleapp consoleapp.cpp)
target_link_libraries(consoleapp PRIVATE ${JOURNAL_LIB})
target_include_directories(consoleapp PRIVATE ${JOURNAL_INCLUDE_DIR})
