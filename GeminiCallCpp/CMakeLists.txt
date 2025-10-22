
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("GeminiCallCpp")

find_package(nlohmann_json REQUIRED)

add_executable (${PROJECT_NAME}  "src/main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)