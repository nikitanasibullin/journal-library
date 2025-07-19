cmake_minimum_required(VERSION 3.14)
project(JournalApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# path to lib
set(JOURNAL_LIB "${CMAKE_SOURCE_DIR}/../src/build/libjournal.a")



add_library(journal STATIC IMPORTED)
set_target_properties(journal PROPERTIES IMPORTED_LOCATION ${JOURNAL_LIB})

# path to include
set(JOURNAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../include")


# linking console app
add_executable(consoleapp consoleapp.cpp)
target_link_libraries(consoleapp PRIVATE journal)
target_include_directories(consoleapp PRIVATE ${JOURNAL_INCLUDE_DIR})
