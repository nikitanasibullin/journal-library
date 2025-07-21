cmake_minimum_required(VERSION 3.14)
project(JournalSocketApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# path to lib
find_library(JOURNAL_LIB journal HINTS "${CMAKE_SOURCE_DIR}/../src/build/lib")
if(NOT JOURNAL_LIB)
    message(FATAL_ERROR "journal library not found")
endif()

# path to include
set(JOURNAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../include")


# linking console app
add_executable(socket_consoleapp socket_consoleapp.cpp)
target_link_libraries(socket_consoleapp PRIVATE ${JOURNAL_LIB})
target_include_directories(socket_consoleapp PRIVATE ${JOURNAL_INCLUDE_DIR})
