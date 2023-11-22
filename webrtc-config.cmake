@PACKAGE_INIT@

# Include the targets file. This file contains definitions of imported targets.
if(NOT TARGET webrtc)
    include("${CMAKE_CURRENT_LIST_DIR}/webrtcTargets.cmake")
endif()