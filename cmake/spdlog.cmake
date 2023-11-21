FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(spdlog)
include_directories(${spdlog_SOURCE_DIR}/include)