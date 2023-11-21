FetchContent_Declare(
    ixwebsocket
    GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
    GIT_TAG        v11.4.4
    GIT_SHALLOW    TRUE
)
set(USE_TLS ON CACHE BOOL "" FORCE)
if(WIN32)
    set(USE_OPEN_SSL ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(ixwebsocket)

