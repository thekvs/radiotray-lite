cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(radiotray_lite)

include(FindPkgConfig)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CPack)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

add_compile_options(-Wall)
add_compile_options(-Wextra)

check_c_compiler_flag("-std=c11" COMPILER_SUPPORTS_C11)
if(NOT COMPILER_SUPPORTS_C11)
    message(FATAL_ERROR "The compiler ${CMAKE_C_COMPILER} has no C11 support.")
endif()

check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

find_package(Threads REQUIRED)
find_package(LibNotify REQUIRED)

pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
pkg_check_modules(APPINDICATOR REQUIRED appindicator3-0.1>=12.10)
pkg_check_modules(CURL REQUIRED libcurl)

pkg_check_modules(GSTREAMER QUIET gstreamer-0.10)
if (NOT GSTREAMER_FOUND)
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
endif()

pkg_check_modules(GSTREAMERMM QUIET gstreamermm-0.10)
if (NOT GSTREAMERMM_FOUND)
    pkg_check_modules(GSTREAMERMM REQUIRED gstreamermm-1.0)
endif()

add_subdirectory(src)

install(
    DIRECTORY data/images DESTINATION share/radiotray-lite/
)
install(
    FILES data/bookmarks.xml DESTINATION share/radiotray-lite/
)

if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
endif()

set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

string(TIMESTAMP BUILDTIME "%Y%m%d%H%M%S")
configure_file(${PROJECT_SOURCE_DIR}/cmake/CPackConfig.cmake.in CPackConfig.cmake @ONLY)
