#include <wayland-egl.h>
#include <EGL/egl.h>

class EGLWaylandContext {
public:
    EGLDisplay display;

    EGLContext context;

    wl_egl_window *window;

    EGLSurface surface;
public:
    ~EGLWaylandContext();
    void init();
};
