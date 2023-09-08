#include "WaylandContext.h"
#include "EGLWaylandContext.h"
#include "XDGContext.h"
#include "xdg-shell-client-protocol.h"
#include "GL/gl3w.h"
#include <EGL/egl.h>

#include <cstring>
#include <poll.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

extern WaylandContext* wlCtx;
extern XDGContext* xdgCtx;
extern EGLWaylandContext* eglCtx;

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
					 uint32_t serial, struct wl_surface *surface,
					 wl_fixed_t sx, wl_fixed_t sy) {
	fprintf(stderr, "Pointer entered surface %p at %f %f\n", surface, wl_fixed_to_double(sx), wl_fixed_to_double(sy));
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
					 uint32_t serial, struct wl_surface *surface) {
	fprintf(stderr, "Pointer left surface %p\n", surface);
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
					  uint32_t time, wl_fixed_t sx, wl_fixed_t sy) {
}

static void pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
					  uint32_t serial, uint32_t time, uint32_t button,
					  uint32_t state) {
	printf("Pointer button\n");
	//if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
	//	xdg_toplevel_move(toplevel, seat, serial);
}

static void pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
					uint32_t time, uint32_t axis, wl_fixed_t value) {
	printf("Pointer handle axis\n");
}

static const struct wl_pointer_listener pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

static void seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t caps) {
	
	if ((caps & WL_SEAT_CAPABILITY_POINTER) && !wlCtx->pointer) {
		wlCtx->pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(wlCtx->pointer, &pointer_listener, NULL);
	}
	else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && wlCtx->pointer)
	{
	}
	
}

static const struct wl_seat_listener seat_listener = {
	seat_handle_capabilities,
};


static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version) {
	std::cout << "Got a registry event for " << interface << "id: " << id << std::endl;
	
    if (strcmp(interface, "wl_compositor") == 0) {
		wlCtx->compositor = (wl_compositor *) wl_registry_bind(registry, id, &wl_compositor_interface, version);
	}
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		xdgCtx->shell = (xdg_wm_base *) wl_registry_bind(registry, id, &xdg_wm_base_interface, version);
		xdg_wm_base_add_listener(xdgCtx->shell, &xdg_wm_base_listener, data);
	}
	else if (strcmp(interface, "wl_seat") == 0)	{
		wlCtx->seat = (struct wl_seat *) wl_registry_bind(registry, id, &wl_seat_interface, 1);
		wl_seat_add_listener(wlCtx->seat, &seat_listener, NULL);
	}
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
	printf("Got a registry losing event for %d\n", id);
}

static const wl_registry_listener registry_listener = {
	global_registry_handler,
	global_registry_remover,
};

void Render(void);

static void frame_handle_done(void *data, struct wl_callback *callback, uint32_t time) {
	wl_callback_destroy(callback);
	Render();
}

const struct wl_callback_listener frame_listener = {
	.done = frame_handle_done,
};

void Render(void) {

	std::cout << "Render" << std::endl;

	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapInterval(eglCtx->display, 0);
	struct wl_callback *callback = wl_surface_frame(wlCtx->surface);
	wl_callback_add_listener(callback, &frame_listener, NULL);

	// This call won't block
	eglSwapBuffers(eglCtx->display, eglCtx->surface);
}

void WaylandContext::init() {
    std:: cout << "XDG_RUNTIME_DIR: " << getenv("XDG_RUNTIME_DIR") << "\n";

	display = wl_display_connect(nullptr);
    assert(display);
	
	std::cout << "connected to display\n";

	fd = wl_display_get_fd(display);

    registry = wl_display_get_registry(display);
    assert(registry);

	wl_registry_add_listener(registry, &registry_listener, nullptr);

	wl_display_roundtrip(display);
	//wl_display_dispatch(display);

    assert(compositor);

    surface = wl_compositor_create_surface(compositor);
    assert(surface);

	// region用来告诉compositor哪些区域不透明，以及哪里接受输入，不是必须的。
	/*
  	region = wl_compositor_create_region(compositor);
	wl_region_add(region, 0, 0, 100, 100);
	wl_surface_set_opaque_region(surface, region);
	*/
}

void WaylandContext::run() {
    std::cout << "Start run" << std::endl;

	struct wl_callback *callback = wl_surface_frame(surface);
	wl_callback_add_listener(callback, &frame_listener, NULL);

    while (true) {
        while (true) {
		    int r = wl_display_dispatch_pending(display);
			assert(r>=0);
			wl_display_flush(display);
			if (wl_display_prepare_read(display) == 0)
			    break;
		}

		pollfd fds[1] = {{fd, POLLIN, 0}};
		int r = poll(fds, 1, -1);
		assert(fds[0].revents & POLLIN);
		
		wl_display_read_events(display); 
	}
}

WaylandContext::~WaylandContext() {
    wl_seat_destroy(seat);
    wl_region_destroy(region);
    wl_pointer_destroy(pointer);
    wl_surface_destroy(surface);
	wl_compositor_destroy(compositor);
	wl_registry_destroy(registry);
	wl_display_disconnect(display);
}
