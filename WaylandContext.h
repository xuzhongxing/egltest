#include <wayland-client.h>

class WaylandContext {
public:
    wl_display* display;

    wl_registry* registry;

    wl_compositor* compositor;

    wl_seat* seat;

    wl_surface* surface;

    wl_pointer* pointer;

    wl_region* region;

    int fd = -1;

public:
    ~WaylandContext();

    void init();

    void flush() {
        wl_display_flush(display);
    }

    void dispatch() {
        wl_display_dispatch(display);
    }

    void run();

};
