#include "XDGContext.h"
#include "WaylandContext.h"
#include "EGLWaylandContext.h"
#include "GL/gl3w.h"
#include <iostream>
#include <cassert>

extern WaylandContext* wlCtx;
extern EGLWaylandContext* eglCtx;
extern XDGContext* xdgCtx;

static void handle_configure(void *data, struct xdg_surface *surface, uint32_t serial) {
	xdg_surface_ack_configure(surface, serial);

	std::cout << "xdg surface handle configure" << std::endl;

	xdgCtx->waitConf = false;
}

static const struct xdg_surface_listener surface_listener = {
	.configure = handle_configure
};

static void xdg_toplevel_handle_configure(void *data, xdg_toplevel *xdg_toplevel, int32_t w, int32_t h,	wl_array *states) {
	std::cout << "xdg_toplevel_handle_configure: " << w << " " << h << std::endl;

	// no window geometry event, ignore
	if (w == 0 && h == 0) return;

	// window resized
	wl_egl_window_resize(eglCtx->window, w, h, 0, 0);

	glClearColor(1, 1, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(eglCtx->display, eglCtx->surface);
}

static void xdg_toplevel_handle_close(void *data, xdg_toplevel *xdg_toplevel) {
	// window closed, be sure that this event gets processed
}

static void configure_bounds(void *data, xdg_toplevel *xdg_toplevel, int32_t width, int32_t height) {
}

struct xdg_toplevel_listener xdg_toplevel_listener = {
	.configure = xdg_toplevel_handle_configure,
	.close = xdg_toplevel_handle_close,
	.configure_bounds = configure_bounds
};

void XDGContext::init() {
    std::cout << "XDG init\n";
    assert(shell);

    surface = xdg_wm_base_get_xdg_surface(shell, wlCtx->surface);
    assert(surface);
    xdg_surface_add_listener(surface, &surface_listener, NULL);

    toplevel = xdg_surface_get_toplevel(surface);
    assert(toplevel);
    xdg_toplevel_set_title(toplevel, "Wayland EGL example");
    xdg_toplevel_add_listener(toplevel, &xdg_toplevel_listener, NULL);
}

void XDGContext::setFullscreen() {
    xdg_toplevel_set_fullscreen(toplevel, NULL);
	wlCtx->flush();
}

void XDGContext::waitForConfigure() {
    wlCtx->flush();
    
    while (waitConf) {
        std::cout << "xdg wait for configure\n";
        wlCtx->dispatch();
    }
}

XDGContext::~XDGContext() {
    xdg_toplevel_destroy(toplevel);
    xdg_surface_destroy(surface);
	xdg_wm_base_destroy(shell);
}
