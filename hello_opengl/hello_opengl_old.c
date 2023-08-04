// example how to set up modern OpenGL context with fallback to legacy context
// compile: gcc xlib_opengl.c -lX11 -lGL -o xlib_opengl

// set to 0 to create resizable window
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

// do you need depth buffer?
#define WINDOW_DEPTH 1

// do you need stencil buffer?
#define WINDOW_STENCIL 0

// use sRGB for color buffer
#define WINDOW_SRGB 1

// do you need multisampling?
// to disable set to 0, to enable set to 2, 4, 8, 16, ...
#define WINDOW_MSAA 4

// do you need vsync?
#define WINDOW_VSYNC 1

// keep this enabled when debugging
#define USE_DEBUG_MODE 1

// replace this with your favorite assert() implementation
#include <assert.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "glext.h"  // download from https://www.opengl.org/registry/api/GL/glext.h
#include "glxext.h" // download from https://www.opengl.org/registry/api/GL/glxext.h

// https://www.opengl.org/registry/specs/ARB/glx_create_context.txt
// https://cgit.freedesktop.org/mesa/mesa/plain/docs/specs/MESA_swap_control.spec
// https://www.opengl.org/registry/specs/SGI/swap_control.txt
// https://www.opengl.org/registry/specs/EXT/swap_control.txt
// https://www.opengl.org/registry/specs/EXT/glx_swap_control_tear.txt
// https://www.opengl.org/registry/specs/ARB/framebuffer_sRGB.txt
// https://www.opengl.org/registry/specs/ARB/multisample.txt
// https://www.opengl.org/registry/specs/ARB/debug_output.txt

#if USE_DEBUG_MODE
    #define GL_CHECK(x) do          \
    {                               \
        x;                          \
        GLenum err = glGetError();  \
        assert(err == GL_NO_ERROR); \
    } while (0)

#else
    #define GL_CHECK(x) x
#endif

// called after context is set up (only once)
// for example, load GL extensions here & set up GL state
static void RenderInit(void)
{
    GL_CHECK( glClearColor(100.f/255.f, 149.f/255.f, 237.f/255.f, 1.f) );
}

// called before render is destroyed
static void RenderDone(void)
{
}

// called when window is resized
static void RenderResize(unsigned width, unsigned height)
{
    GL_CHECK( glViewport(0, 0, width, height) );
}

// called every frame before swapping buffers
static void RenderFrame(void)
{
    GL_CHECK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) );

    // use legacy GL for drawing to display a triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex2f(0.f, 0.5f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex2f(0.5f, -0.5f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex2f(-0.5f, -0.5f);
    GL_CHECK( glEnd() );
}

#if USE_DEBUG_MODE
static void APIENTRY OpenGLDebugCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar *message, const void* user)
{
    fprintf(stderr, "%s\n", message);
    if (severity >= GL_DEBUG_SEVERITY_LOW_ARB && severity <= GL_DEBUG_SEVERITY_HIGH_ARB)
    {
        assert(0);
    }
}
#endif

static int StringsAreEqual(const char* src, const char* dst, size_t dstlen)
{
    while (*src && dstlen-- && *dst)
    {
        if (*src++ != *dst++)
        {
            return 0;
        }
    }

    return (dstlen && *src == *dst) || (!dstlen && *src == 0);
}

int main(void)
{
    Display* display = XOpenDisplay(NULL);
    if (!display)
    {
        fprintf(stderr, "XOpenDisplay failed!\n");
    }
    else
    {
        int glx_ARB_create_context = 0;
        int glx_ARB_multisample = 0;
        int glx_ARB_framebuffer_sRGB = 0;
        int glx_SGI_swap_control = 0;
        int glx_MESA_swap_control = 0;
        int glx_EXT_swap_control = 0;
        int glx_EXT_swap_control_tear = 0;

        const char* glx = glXQueryExtensionsString(display, DefaultScreen(display));
        if (!glx)
        {
            fprintf(stderr, "glXQueryExtensionsString failed!\n");
        }
        else
        {
            const char* start = glx;
            for (;;)
            {
                while (*glx != 0 && *glx != ' ')
                {
                    glx++;
                }

                size_t length = glx - start;
                if (StringsAreEqual("GLX_ARB_create_context", start, length))
                {
                    glx_ARB_create_context = 1;
                }
                else if (StringsAreEqual("GLX_ARB_multisample", start, length))
                {
                    glx_ARB_multisample = 1;
                }
                else if (StringsAreEqual("GLX_ARB_framebuffer_sRGB", start, length))
                {
                    glx_ARB_framebuffer_sRGB = 1;
                }
                else if (StringsAreEqual("GLX_SGI_swap_control", start, length))
                {
                    glx_SGI_swap_control = 1;
                }
                else if (StringsAreEqual("GLX_MESA_swap_control", start, length))
                {
                    glx_MESA_swap_control = 1;
                }
                else if (StringsAreEqual("GLX_EXT_swap_control", start, length))
                {
                    glx_EXT_swap_control = 1;
                }
                else if (StringsAreEqual("GLX_EXT_swap_control_tear", start, length))
                {
                    glx_EXT_swap_control_tear = 1;
                }

                if (*glx == 0)
                {
                    break;
                }

                glx++;
                start = glx;
            }
        }

        int attr[32];
        int* p = attr;
        *p++ = GLX_X_VISUAL_TYPE; *p++ = GLX_TRUE_COLOR;
        *p++ = GLX_DOUBLEBUFFER;  *p++ = True;
        *p++ = GLX_RED_SIZE;      *p++ = 8;
        *p++ = GLX_GREEN_SIZE;    *p++ = 8;
        *p++ = GLX_BLUE_SIZE;     *p++ = 8;
        *p++ = GLX_DEPTH_SIZE;    *p++ = WINDOW_DEPTH ? 24 : 0;
        *p++ = GLX_STENCIL_SIZE;  *p++ = WINDOW_STENCIL ? 8 : 0;
        if (WINDOW_SRGB && glx_ARB_framebuffer_sRGB)
        {
            *p++ = GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB;
            *p++ = True;
        }
        if (WINDOW_MSAA && glx_ARB_multisample)
        {
            *p++ = GLX_SAMPLE_BUFFERS_ARB;
            *p++ = 1;
            *p++ = GLX_SAMPLES_ARB;
            *p++ = WINDOW_MSAA;
        }
        *p++ = 0;

        int count;
        GLXFBConfig* config = glXChooseFBConfig(display, DefaultScreen(display), attr, &count);
        if (!config || count == 0)
        {
            fprintf(stderr, "glXChooseFBConfig failed!\n");
        }
        else
        {
            XVisualInfo* info = glXGetVisualFromFBConfig(display, config[0]);
            if (!info)
            {
                fprintf(stderr, "glXGetVisualFromFBConfig failed!\n");
            }
            else
            {
                Window root = DefaultRootWindow(display);

                Colormap cmap = XCreateColormap(display, root, info->visual, AllocNone);
                if (!cmap)
                {
                    fprintf(stderr, "XCreateColormap failed!\n");
                }
                else
                {
                    XSetWindowAttributes swa;
                    swa.colormap = cmap;
                    swa.event_mask = StructureNotifyMask;

                    Window window = XCreateWindow(
                        display, root, 0, 0,
                        WINDOW_WIDTH ? WINDOW_WIDTH : 800,
                        WINDOW_HEIGHT ? WINDOW_HEIGHT : 450,
                        0, info->depth, InputOutput, info->visual,
                        CWColormap | CWEventMask, &swa);
                        XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask);
                    if (!window)
                    {
                        fprintf(stderr, "XCreateWindow failed!\n");
                    }
                    else
                    {
                        if (WINDOW_WIDTH && WINDOW_HEIGHT)
                        {
                            XSizeHints* hints = XAllocSizeHints();
                            if (!hints)
                            {
                                fprintf(stderr, "XAllocSizeHints failed!\n");
                            }
                            else
                            {
                                hints->flags |= PMinSize | PMaxSize;
                                hints->min_width = hints->max_width = WINDOW_WIDTH;
                                hints->min_height = hints->max_height = WINDOW_HEIGHT;
                                XSetWMNormalHints(display, window, hints);
                                XFree(hints);
                            }
                        }
                        Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
                        XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);

                        XStoreName(display, window, "OpenGL window");
                        XMapWindow(display, window);

                        GLXContext ctx = NULL;
                        if (glx_ARB_create_context)
                        {
                            int cattr[] =
                            {
                                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
#if USE_DEBUG_MODE
                                GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
#endif
                                GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                                None,
                            };

                            PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
                                (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB(
                                    (const GLubyte*)"glXCreateContextAttribsARB");
                            ctx = glXCreateContextAttribsARB(display, config[0], 0, True, cattr);
                            if (!ctx)
                            {
                                fprintf(stderr, "Failed to create modern OpenGL context, trying legacy context!\n");
                            }
                        }

                        if (!ctx)
                        {
                            ctx = glXCreateContext(display, info, NULL, GL_TRUE);
                        }

                        if (!ctx)
                        {
                            fprintf(stderr, "glXCreateContext failed!\n");
                        }
                        else
                        {
                            if (!glXMakeCurrent(display, window, ctx))
                            {
                                fprintf(stderr, "glXMakeCurrent failed!\n");
                            }
                            else
                            {
                                if (WINDOW_VSYNC)
                                {
                                    if (glx_EXT_swap_control)
                                    {                 
                                        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT =
                                            (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB(
                                                (const GLubyte*)"glXSwapIntervalEXT");
                                        glXSwapIntervalEXT(display, window, glx_EXT_swap_control_tear ? -1 : 1);
                                    }
                                    else if (glx_MESA_swap_control)
                                    {
                                        PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA =
                                            (PFNGLXSWAPINTERVALMESAPROC)glXGetProcAddressARB(
                                                (const GLubyte*)"glXSwapIntervalMESA");
                                        if (glXSwapIntervalMESA(1))
                                        {
                                            fprintf(stderr, "glXSwapIntervalMESA failed!\n");
                                        }
                                    }
                                    else if (glx_SGI_swap_control)
                                    {
                                        PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI =
                                            (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddressARB(
                                                (const GLubyte*)"glXSwapIntervalSGI");
                                        if (glXSwapIntervalSGI(1))
                                        {
                                            fprintf(stderr, "glXSwapIntervalSGI failed!\n");
                                        }
                                    }
                                }

#if USE_DEBUG_MODE
                                const GLubyte* ext;
                                GL_CHECK( ext = glGetString(GL_EXTENSIONS) );
                                if (ext)
                                {
                                    const GLubyte* start = ext;
                                    for (;;)
                                    {
                                        while (*ext != 0 && *ext != ' ')
                                        {
                                            ext++;
                                        }

                                        size_t length = ext - start;
                                        if (StringsAreEqual("GL_ARB_debug_output", (const char*)start, length))
                                        {
                                            PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB =
                                                (PFNGLDEBUGMESSAGECALLBACKARBPROC)glXGetProcAddressARB((const GLubyte*)"glDebugMessageCallbackARB");
                                            GL_CHECK( glDebugMessageCallbackARB(OpenGLDebugCallback, NULL) );
                                            GL_CHECK( glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB) );
                                            break;
                                        }

                                        if (*ext == 0)
                                        {
                                            break;
                                        }

                                        ext++;
                                        start = ext;
                                    }
                                }
#endif
                                if (WINDOW_MSAA && glx_ARB_multisample)
                                {
                                    GL_CHECK( glEnable(GL_MULTISAMPLE_ARB) );
                                }                

                                RenderInit();

                                int Running = 1;
                                while (Running)
                                {
                                    if (XPending(display))
                                    {
                                        XEvent event;
                                        XNextEvent(display, &event);

                                        if (event.type == ConfigureNotify)
                                        {
                                            RenderResize(event.xconfigure.width, event.xconfigure.height);
                                        }
                                        else if (event.type == ClientMessage)
                                        {
                                            if ((Atom)event.xclient.data.l[0] == WM_DELETE_WINDOW)
                                            {
                                                break;
                                            }
                                        }
                                        else if((event.type == KeyPress) &&
                                                (event.xkey.keycode == 9)) // KEYCODE_ESCAPE
                                        {
                                            Running = 0;
                                        }

                                        continue;
                                    }

                                    RenderFrame();

                                    glXSwapBuffers(display, window);
                                }

                                RenderDone();

                                glXMakeCurrent(display, None, NULL);
                            }
                            glXDestroyContext(display, ctx);
                        }
                        XDestroyWindow(display, window);
                    }
                    XFreeColormap(display, cmap);
                }
                XFree(info);
            }
            XFree(config);
        }
        XCloseDisplay(display);
    }

    return 0;
}