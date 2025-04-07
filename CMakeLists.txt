cmake_minimum_required(VERSION 3.10)
project(ServerProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

file(GLOB SOURCES "src/*.cpp")

add_executable(server ${SOURCES})

target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)

if(WIN32)
    target_link_libraries(server ws2_32)
endif()