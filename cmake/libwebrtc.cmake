add_definitions(-DRTC_DESKTOP_DEVICE)
set(LIBWEBRTC_VERSION "0.1.0")

if(WIN32)
    set(FILAMENT_URL "https://github.com/szpnygo/libwebrtc/releases/download/v${LIBWEBRTC_VERSION}/libwebrtc-win-x64.zip")
elseif(APPLE)
    set(FILAMENT_URL "https://github.com/szpnygo/libwebrtc/releases/download/v${LIBWEBRTC_VERSION}/libwebrtc-mac-x64.zip")
else()
    set(FILAMENT_URL "https://github.com/szpnygo/libwebrtc/releases/download/v${LIBWEBRTC_VERSION}/libwebrtc-linux-x64.zip")
endif()

FetchContent_Declare(
    libwebrtc
    URL ${FILAMENT_URL}
)
FetchContent_MakeAvailable(libwebrtc)