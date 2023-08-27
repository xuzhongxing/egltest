cmake_minimum_required (VERSION 3.25)

project (egltest)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(windowegl window_egl.cpp gl3w.c xdg-shell-protocol.c)

target_link_libraries(windowegl wayland-client wayland-egl EGL GL)
