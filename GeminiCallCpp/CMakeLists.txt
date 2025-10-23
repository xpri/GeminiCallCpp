
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("GeminiCallCpp")

find_package(nlohmann_json REQUIRED)
find_package(curl REQUIRED)

add_executable (${PROJECT_NAME}  "src/main.cpp" "src/config.cpp" "src/helper.cpp" "src/helper.h")

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)