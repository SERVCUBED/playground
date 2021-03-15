cmake_minimum_required(VERSION 3.10)
project(keypad-macros)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GDK gdk-3.0)
pkg_check_modules(WNCK libwnck-3.0)
pkg_check_modules(LIBNOTIFY libnotify)
pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(XCB xcb xcb-aux xcb-xtest)

#link_directories(${GDK_LIBRARY_DIRS})

add_executable(keypad-macros keypad-macros.cpp)
target_link_libraries(keypad-macros ${GDK_LIBRARIES})
include_directories(keypad-macros ${GDK_INCLUDE_DIRS})

#project(focus-window-strstr)

add_executable(focus-window-strstr focus-window-strstr.cpp)
target_link_libraries(focus-window-strstr ${GDK_LIBRARIES} ${WNCK_LIBRARIES})
include_directories(focus-window-strstr ${GDK_LIBRARIES} ${WNCK_INCLUDE_DIRS})

add_executable(notify-usage notify-usage.cpp)
target_link_libraries(notify-usage ${GDK_LIBRARIES} ${LIBNOTIFY_LIBRARIES})
include_directories(notify-usage ${GDK_LIBRARIES} ${LIBNOTIFY_INCLUDE_DIRS})

add_executable(mouse-do mouse-do.cpp)
target_link_libraries(mouse-do ${GTKMM_LIBRARIES} ${XCB_LIBRARIES})
include_directories(mouse-do ${GTKMM_INCLUDE_DIRS} ${XCB_INCLUDE_DIRS})

add_executable(warn-bar warn-bar.cpp)
target_link_libraries(warn-bar ${GTKMM_LIBRARIES} ${XCB_LIBRARIES})
include_directories(warn-bar ${GTKMM_INCLUDE_DIRS} ${XCB_INCLUDE_DIRS})
