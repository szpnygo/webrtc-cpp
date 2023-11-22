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
    src/connection.cpp
    src/stream.cpp
)

# add library
add_library(${CMAKE_PROJECT_NAME} STATIC ${FILES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${libwebrtc_SOURCE_DIR}/include)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ixwebsocket)

set(LIBWEBRTC_PATH "${libwebrtc_SOURCE_DIR}/lib")
if(WIN32)
    set(LIBWEBRTC_FILE "${LIBWEBRTC_PATH}/libwebrtc.dll.lib")
elseif(LINUX)
    set(LIBWEBRTC_FILE "${LIBWEBRTC_PATH}/libwebrtc.so")
elseif(APPLE)
    set(LIBWEBRTC_FILE "${LIBWEBRTC_PATH}/libwebrtc.dylib")
endif()
message(STATUS "libwebrtc: ${LIBWEBRTC_FILE}")
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${LIBWEBRTC_FILE})

# add executable
add_executable(${CMAKE_PROJECT_NAME}_example example/main.cpp example/video.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}_example PUBLIC ${CMAKE_PROJECT_NAME})


# install library
install(TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT webrtcTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

# create and install config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "webrtcConfigVersion.cmake"
    VERSION 1.0
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/webrtcConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/webrtcConfig.cmake"
    INSTALL_DESTINATION lib/cmake/webrtc
)

install(EXPORT webrtcTargets
        FILE webrtcTargets.cmake
        DESTINATION lib/cmake/webrtc)

install(FILES
    "${CMAKE_BINARY_DIR}/webrtcConfig.cmake"
    "${CMAKE_BINARY_DIR}/webrtcConfigVersion.cmake"
    DESTINATION lib/cmake/webrtc)