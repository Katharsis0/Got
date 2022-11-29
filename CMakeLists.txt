cmake_minimum_required(VERSION 3.23)
project(CLI)

set(CMAKE_CXX_STANDARD 14)
find_package(CURL REQUIRED)
add_executable(
        main.cpp src/CommandParser.cpp include/CommandParser.h include/json.hpp include/API.h src/API.cpp)

include_directories(${CURL_INCLUDE_DIR})

link_directories(/usr/bin/curl /usr/share/man/man1/curl.1.gz)

target_link_libraries(got ${CURL_LIBRARIES})
