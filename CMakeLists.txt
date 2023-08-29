cmake_minimum_required (VERSION 3.25)

project (egltest)

include_directories(${CMAKE_SOURCE_DIR})

add_compile_options(-g)

add_executable(windowegl window_egl.cpp WaylandContext.cpp XDGContext.cpp EGLWaylandContext.cpp gl3w.c xdg-shell-protocol.c)

target_link_libraries(windowegl wayland-client wayland-egl EGL GL)

#add_executable(example example.c xdg-shell-protocol.c)

#target_link_libraries(example wayland-client wayland-egl EGL GL)
