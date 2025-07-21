cmake_minimum_required(VERSION 3.14)
project(JournalTests LANGUAGES CXX)

# set c++ standard to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# fetch googletest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# find journal library
find_library(journal_lib journal HINTS "${CMAKE_SOURCE_DIR}/../src/build/lib")
if(NOT journal_lib)
    message(FATAL_ERROR "journal library not found")
endif()

# create tests executable
add_executable(tests tests_journal.cpp)

# link libraries
target_link_libraries(tests PRIVATE ${journal_lib} gtest gtest_main)

# path to include dir
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/../include)

# enable testing
enable_testing()
add_test(NAME JournalTests COMMAND tests)
