cmake_minimum_required(VERSION 3.26)
project(webrtc)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

# install deps
include(cmake/libwebrtc.cmake)
include(cmake/ixwebsocket.cmake)
include(cmake/json.cmake)
include(cmake/spdlog.cmake)

set(FILES
    src/webrtc.cpp
    src/signal.cpp
)

# add library
add_library(${CMAKE_PROJECT_NAME} STATIC ${FILES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${libwebrtc_SOURCE_DIR}/include)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${libwebrtc_SOURCE_DIR}/lib/libwebrtc.dll.lib)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ixwebsocket)

# add executable
add_executable(${CMAKE_PROJECT_NAME}_example example/main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}_example PUBLIC ${CMAKE_PROJECT_NAME})
