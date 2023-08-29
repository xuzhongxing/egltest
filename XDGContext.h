#include "xdg-shell-client-protocol.h"

class XDGContext {
public:
    xdg_wm_base* shell;

    xdg_toplevel *toplevel;

    xdg_surface* surface;

    bool waitConf = true;

public:
    ~XDGContext();
    void init();

    void setFullscreen();

    void waitForConfigure();
};
