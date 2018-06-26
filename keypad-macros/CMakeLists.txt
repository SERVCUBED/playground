cmake_minimum_required(VERSION 3.10)
project(keypad_macros)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GDK gdk-3.0 libwnck-3.0)

include_directories(${GDK_INCLUDE_DIRS})
link_directories(${GDK_LIBRARY_DIRS})

add_executable(keypad_macros main.cpp)
target_link_libraries(keypad_macros ${GDK_LIBRARIES})