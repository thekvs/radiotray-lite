include_directories(${APPINDICATOR_INCLUDE_DIRS})
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(third_party)

add_executable(
    radiotray-lite
    radiotray-lite.cpp
    tray.cpp
    third_party/pugixml/pugixml.cpp
)
target_link_libraries(radiotray-lite ${GTKMM_LIBRARIES} ${APPINDICATOR_LIBRARIES})
set_target_properties(radiotray-lite PROPERTIES COMPILE_FLAGS "-std=c++11")

