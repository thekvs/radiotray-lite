include_directories(${LIBXML2_INCLUDE_DIRS})
include_directories(${APPINDICATOR_INCLUDE_DIRS})
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${GSTREAMERMM_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${LIBNOTIFY_INCLUDE_DIRS})
include_directories(third_party)

add_definitions(-Wno-deprecated-declarations)

add_executable(
    radiotray-lite
    radiotray-lite.cpp
    tray.cpp
    player.cpp
    about.cpp
    playlist.cpp
    notification.cpp
    m3u_playlist_decoder.cpp
    pls_playlist_decoder.cpp
    asx_playlist_decoder.cpp
    third_party/pugixml/pugixml.cpp
)
target_link_libraries(
    radiotray-lite
    ${GSTREAMERMM_LIBRARIES}
    ${GTKMM_LIBRARIES}
    ${CURL_LIBRARIES}
    ${APPINDICATOR_LIBRARIES}
    ${LIBNOTIFY_LIBRARIES}
)
set_target_properties(radiotray-lite PROPERTIES COMPILE_FLAGS "-std=c++11")
target_compile_definitions(radiotray-lite PRIVATE -DELPP_DISABLE_DEFAULT_CRASH_HANDLING)
target_compile_definitions(radiotray-lite PRIVATE -DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

add_executable(
    radiotray-lite-cli
    playradio.cpp
    player.cpp
    playlist.cpp
    m3u_playlist_decoder.cpp
    pls_playlist_decoder.cpp
    asx_playlist_decoder.cpp
)
target_link_libraries(
    radiotray-lite-cli
    ${GSTREAMERMM_LIBRARIES}
    ${CURL_LIBRARIES}
)
set_target_properties(radiotray-lite-cli PROPERTIES COMPILE_FLAGS "-std=c++11")
target_compile_definitions(radiotray-lite-cli PRIVATE -DELPP_DISABLE_DEFAULT_CRASH_HANDLING)
target_compile_definitions(radiotray-lite-cli PRIVATE -DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

install(
    TARGETS radiotray-lite radiotray-lite-cli
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
