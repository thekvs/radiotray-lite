cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(radiotray_lite)

include(FindPkgConfig)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
pkg_check_modules(APPINDICATOR REQUIRED appindicator3-0.1>=12.10)

add_subdirectory(src)
