#include "EGLWaylandContext.h"
#include "WaylandContext.h"
#include "GL/gl3w.h"
#include <stdio.h>

extern WaylandContext* wlCtx;

EGLWaylandContext::~EGLWaylandContext() {
    wl_egl_window_destroy(window);
}

void EGLWaylandContext::init() {
    EGLint major, minor, config_count, tmp_n;

	EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT, 
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_NONE};
	EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE};

	EGLConfig config;

	display = eglGetDisplay(wlCtx->display);
	eglInitialize(display, &major, &minor);
	printf("EGL major: %d, minor %d\n", major, minor);
	eglGetConfigs(display, 0, 0, &config_count);
	printf("EGL has %d configs\n", config_count);
	eglChooseConfig(display, config_attribs, &config, 1, &tmp_n);

	// This is important. Without this GLES will be chosen.
    eglBindAPI(EGL_OPENGL_API);

	context = eglCreateContext(display, config, EGL_NO_CONTEXT,	context_attribs);

	// init window
	window = wl_egl_window_create(wlCtx->surface, 100, 100);
	
    surface = eglCreateWindowSurface(display, config, window, 0);
	eglMakeCurrent(display, surface, surface, context);

	int res = gl3wInit();

	int majorVer;
	int minorVer;

	const char* ven = (const char*) glGetString(GL_VENDOR);
	printf("gl vendor: %s\n", ven);

	const char* r = (const char*) glGetString(GL_RENDERER);
	printf("gl renderer: %s\n", r);

	const char* v = (const char*) glGetString(GL_VERSION);
	printf("gl version: %s\n", v);

    glClearColor(1, 1, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(display, surface);
}
