include_directories(${APPINDICATOR_INCLUDE_DIRS})
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${GSTREAMERMM_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(third_party)

add_executable(
    radiotray-lite
    radiotray-lite.cpp
    tray.cpp
    third_party/pugixml/pugixml.cpp
)
target_link_libraries(radiotray-lite ${GTKMM_LIBRARIES} ${APPINDICATOR_LIBRARIES})
set_target_properties(radiotray-lite PROPERTIES COMPILE_FLAGS "-std=c++11")

add_executable(
    playradio
    playradio.cpp
    player.cpp
    playlist.cpp
    m3u_playlist_decoder.cpp
    pls_playlist_decoder.cpp
    asx_playlist_decoder.cpp
)
target_link_libraries(playradio ${GSTREAMERMM_LIBRARIES} ${CURL_LIBRARIES})
set_target_properties(playradio PROPERTIES COMPILE_FLAGS "-std=c++11")
target_compile_definitions(playradio PRIVATE -DELPP_DISABLE_DEFAULT_CRASH_HANDLING)
