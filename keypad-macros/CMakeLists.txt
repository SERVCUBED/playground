cmake_minimum_required(VERSION 3.10)
project(keypad_macros)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GDK gdk-3.0 libwnck-3.0 libnotify)

#link_directories(${GDK_LIBRARY_DIRS})

add_executable(keypad_macros main.cpp)
target_link_libraries(keypad_macros ${GDK_LIBRARIES})
include_directories(keypad_macros ${GDK_INCLUDE_DIRS})

#project(focus-window-strstr)

add_executable(focus-window-strstr focus-window-strstr.cpp)
target_link_libraries(focus-window-strstr ${GDK_LIBRARIES} ${WNCK_LIBRARIES})
include_directories(focus-window-strstr ${GDK_LIBRARIES} ${WNCK_INCLUDE_DIRS})

add_executable(wm-util wm-util.cpp)
target_link_libraries(wm-util ${GDK_LIBRARIES} ${WNCK_LIBRARIES})
include_directories(wm-util ${GDK_LIBRARIES} ${WNCK_INCLUDE_DIRS})

add_executable(quick-focus quick-focus.cpp)
target_link_libraries(quick-focus ${GDK_LIBRARIES} ${WNCK_LIBRARIES})
include_directories(quick-focus ${GDK_LIBRARIES} ${WNCK_INCLUDE_DIRS})

add_executable(notify-usage notify_usage.cpp)
target_link_libraries(notify-usage ${GDK_LIBRARIES} ${LIBNOTIFY_LIBRARIES})
include_directories(notify-usage ${GDK_LIBRARIES} ${LIBNOTIFY_INCLUDE_DIRS})
