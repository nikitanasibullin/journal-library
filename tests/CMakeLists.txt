cmake_minimum_required(VERSION 3.14)
project(JournalTests)

# using gtest
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip


)
FetchContent_MakeAvailable(googletest)

add_executable(tests tests_journal.cpp)

add_library(journal STATIC IMPORTED)
set_target_properties(journal PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../src/build/libjournal.a)

# connecting gtest lib and tests
target_link_libraries(tests PRIVATE journal gtest gtest_main)

# path to include dir
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/../include)

#starting tests
enable_testing()
add_test(NAME JournalTests COMMAND tests)
