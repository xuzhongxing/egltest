#include "WaylandContext.h"
#include "XDGContext.h"
#include "EGLWaylandContext.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/input.h>
#include <iostream>
#include <cassert>

WaylandContext* wlCtx;
XDGContext* xdgCtx;
EGLWaylandContext* eglCtx;

int main(int argc, char **argv)
{
	wlCtx = new WaylandContext();
	xdgCtx = new XDGContext();
	eglCtx = new EGLWaylandContext();

	wlCtx->init();

	xdgCtx->init();

	eglCtx->init();

	//xdgCtx->setFullscreen();

	xdgCtx->waitForConfigure();

	wlCtx->run();

	delete eglCtx;
	delete xdgCtx;
	delete wlCtx;

	return 0;
}
