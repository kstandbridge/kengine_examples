// example how to set up modern OpenGL context with fallback to legacy context

// set to 0 to create resizable window
#define WINDOW_WIDTH 1280
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>

#include "kengine/glext.h"  // download from https://www.opengl.org/registry/api/GL/glext.h
#include "kengine/wglext.h" // download from https://www.opengl.org/registry/api/GL/wglext.h

// https://www.opengl.org/registry/specs/ARB/wgl_extensions_string.txt
// https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt
// https://www.opengl.org/registry/specs/ARB/wgl_create_context.txt
// https://www.opengl.org/registry/specs/EXT/wgl_swap_control.txt
// https://www.opengl.org/registry/specs/EXT/wgl_swap_control_tear.txt
// https://www.opengl.org/registry/specs/ARB/framebuffer_sRGB.txt
// https://www.opengl.org/registry/specs/ARB/multisample.txt
// https://www.opengl.org/registry/specs/ARB/debug_output.txt

#pragma comment (lib, "gdi32.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "opengl32.lib")

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

static void LogWin32Error(const char* msg)
{
    OutputDebugStringA(msg);
    OutputDebugStringA("!\n");

    DWORD err = GetLastError();

    LPWSTR str;
    if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
        err, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPWSTR)&str, 0, NULL))
    {
        OutputDebugStringW(str);
        LocalFree(str);
    }
}

static HGLRC CreateOldOpenGLContext(HDC dc)
{
    PIXELFORMATDESCRIPTOR pfd =
    {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                   PFD_DOUBLEBUFFER | (WINDOW_DEPTH ? 0 : PFD_DEPTH_DONTCARE),
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
        .cDepthBits = (WINDOW_DEPTH ? 24 : 0),
        .cStencilBits = (WINDOW_STENCIL ? 8 : 0),
    };

    int format = ChoosePixelFormat(dc, &pfd);
    if (!format)
    {
        LogWin32Error("ChoosePixelFormat failed");
        return NULL;
    }

    if (!DescribePixelFormat(dc, format, sizeof(pfd), &pfd))
    {
        LogWin32Error("DescribePixelFormat failed");
        return NULL;
    }

    if (!SetPixelFormat(dc, format, &pfd))
    {
        LogWin32Error("SetPixelFormat failed");
        return NULL;
    }

    HGLRC rc = wglCreateContext(dc);
    if (!rc)
    {
        LogWin32Error("wglCreateContext failed");
        return NULL;
    }

    if (!wglMakeCurrent(dc, rc))
    {
        LogWin32Error("wglMakeCurrent failed");
        wglDeleteContext(rc);
        return NULL;
    }

    return rc;
}

#if USE_DEBUG_MODE
static void APIENTRY OpenGLDebugCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar *message, const void* user)
{
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
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

static HGLRC CreateOpenGLContext(HDC dc)
{
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    int wgl_ARB_multisample = 0;
    int wgl_ARB_framebuffer_sRGB = 0;
    int wgl_EXT_swap_control_tear = 0;

    HWND wnd = CreateWindowExW(0, L"STATIC", L"Dummy Window", WS_OVERLAPPED,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
    if (wnd)
    {
        HDC dc = GetDC(wnd);
        if (dc)
        {
            HGLRC rc = CreateOldOpenGLContext(dc);
            if (rc)
            {
                PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
                    (void*)wglGetProcAddress("wglGetExtensionsStringARB");
                if (wglGetExtensionsStringARB != (void*)0 &&
                    wglGetExtensionsStringARB != (void*)1 &&
                    wglGetExtensionsStringARB != (void*)2 &&
                    wglGetExtensionsStringARB != (void*)3 &&
                    wglGetExtensionsStringARB != (void*)-1)
                {
                    const char* ext = wglGetExtensionsStringARB(dc);
                    if (ext)
                    {
                        const char* start = ext;
                        for (;;)
                        {
                            while (*ext != 0 && *ext != ' ')
                            {
                                ext++;
                            }

                            size_t length = ext - start;
                            if (StringsAreEqual("WGL_ARB_pixel_format", start, length))
                            {
                                wglChoosePixelFormatARB = (void*)wglGetProcAddress("wglChoosePixelFormatARB");
                            }
                            else if (StringsAreEqual("WGL_ARB_create_context", start, length))
                            {
                                wglCreateContextAttribsARB = (void*)wglGetProcAddress("wglCreateContextAttribsARB");
                            }
                            else if (StringsAreEqual("WGL_EXT_swap_control", start, length))
                            {
                                wglSwapIntervalEXT = (void*)wglGetProcAddress("wglSwapIntervalEXT");
                            }
                            else if (StringsAreEqual("WGL_ARB_framebuffer_sRGB", start, length))
                            {
                                wgl_ARB_framebuffer_sRGB = 1;
                            }
                            else if (StringsAreEqual("WGL_ARB_multisample", start, length))
                            {
                                wgl_ARB_multisample = 1;
                            }
                            else if (StringsAreEqual("WGL_ARB_framebuffer_sRGB", start, length))
                            {
                                wgl_ARB_framebuffer_sRGB = 1;
                            }
                            else if (StringsAreEqual("WGL_EXT_swap_control_tear", start, length))
                            {
                                wgl_EXT_swap_control_tear = 1;
                            }

                            if (*ext == 0)
                            {
                                break;
                            }

                            ext++;
                            start = ext;
                        }
                    }
                }

                wglMakeCurrent(NULL, NULL);
                wglDeleteContext(rc);
            }
            ReleaseDC(wnd, dc);
        }
        DestroyWindow(wnd);
    }

    HGLRC rc = NULL;

    if (wglCreateContextAttribsARB && wglChoosePixelFormatARB)
    {
        int attrib[32];
        int* p = attrib;

        *p++ = WGL_DRAW_TO_WINDOW_ARB; *p++ = GL_TRUE;
        *p++ = WGL_ACCELERATION_ARB;   *p++ = WGL_FULL_ACCELERATION_ARB;
        *p++ = WGL_SUPPORT_OPENGL_ARB; *p++ = GL_TRUE;
        *p++ = WGL_DOUBLE_BUFFER_ARB;  *p++ = GL_TRUE;
        *p++ = WGL_PIXEL_TYPE_ARB;     *p++ = WGL_TYPE_RGBA_ARB;
        *p++ = WGL_COLOR_BITS_ARB;     *p++ = 24;

        if (WINDOW_DEPTH)
        {
            *p++ = WGL_DEPTH_BITS_ARB;
            *p++ = 24;
        }
        if (WINDOW_STENCIL)
        {
            *p++ = WGL_STENCIL_BITS_ARB;
            *p++ = 8;
        }
        if (WINDOW_SRGB && wgl_ARB_framebuffer_sRGB)
        {
            *p++ = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
            *p++ = GL_TRUE;
        }
        if (WINDOW_MSAA && wgl_ARB_multisample)
        {
            *p++ = WGL_SAMPLE_BUFFERS_ARB;
            *p++ = 1;
            *p++ = WGL_SAMPLES_ARB;
            *p++ = WINDOW_MSAA;
        }
        *p = 0;

        int format;
        UINT formats;
        if (!wglChoosePixelFormatARB(dc, attrib, NULL, 1, &format, &formats) || formats == 0)
        {
            LogWin32Error("wglChoosePixelFormatARB failed");
        }
        else
        {
            PIXELFORMATDESCRIPTOR pfd =
            {
                .nSize = sizeof(pfd),
            };

            if (!DescribePixelFormat(dc, format, sizeof(pfd), &pfd))
            {
                LogWin32Error("DescribePixelFormat failed");
                return NULL;
            }
            else
            {
                if (!SetPixelFormat(dc, format, &pfd))
                {
                    LogWin32Error("SetPixelFormat failed");
                }
                else
                {
                    int ctx[] =
                    {
                        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                        WGL_CONTEXT_MINOR_VERSION_ARB, 0,
#if USE_DEBUG_MODE
                        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
                        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                        0,
                    };

                    rc = wglCreateContextAttribsARB(dc, NULL, ctx);
                    if (!rc)
                    {
                        LogWin32Error("wglCreateContextAttribsARB failed");
                    }
                    else
                    {
                        if (!wglMakeCurrent(dc, rc))
                        {
                            LogWin32Error("wglMakeCurrent failed");
                            wglDeleteContext(rc);
                            rc = NULL;
                        }
                        else
                        {
                            if (WINDOW_MSAA && wgl_ARB_multisample)
                            {
                                GL_CHECK(glEnable(GL_MULTISAMPLE_ARB));
                            }
                        }
                    }
                }
            }
        }
    }

    if (!rc)
    {
        OutputDebugStringA("Failed to create modern OpenGL context, retrying with legacy context!\n");
        rc = CreateOldOpenGLContext(dc);
    }

    if (rc)
    {
        if (WINDOW_VSYNC && wglSwapIntervalEXT)
        {
            if (!wglSwapIntervalEXT(wgl_EXT_swap_control_tear ? -1 : 1))
            {
                LogWin32Error("wglSwapIntervalEXT failed");
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
                        (void*)wglGetProcAddress("glDebugMessageCallbackARB");
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
    }

    return rc;
}

struct Win32Context
{
    HDC dc;
    HGLRC rc;
};

static LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    struct Win32Context* ctx = (void*)GetWindowLongPtr(wnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
       ctx = ((CREATESTRUCTW*)lparam)->lpCreateParams;
       SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)ctx);

       if (!(ctx->dc = GetDC(wnd)))
       {
           LogWin32Error("GetDC failed");
           return -1;
       }

       if (!(ctx->rc = CreateOpenGLContext(ctx->dc)))
       {
           ReleaseDC(wnd, ctx->dc);
           return -1;
       }
       RenderInit();
       return 0;

    case WM_DESTROY:
        if (ctx->rc)
        {
            RenderDone();

            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(ctx->rc);
        }
        if (ctx->dc)
        {
            ReleaseDC(wnd, ctx->dc);
        }

        PostQuitMessage(0);
        return 0;
        
    case WM_SIZE:
        RenderResize(LOWORD(lparam), HIWORD(lparam));
        return 0;
    }
    return DefWindowProcW(wnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    WNDCLASSEXW wc =
    {
        .cbSize = sizeof(wc),
        .lpfnWndProc = WindowProc,
        .hInstance = instance,
        .hIcon = LoadIconA(NULL, IDI_APPLICATION),
        .hCursor = LoadCursorA(NULL, IDC_ARROW),
        .lpszClassName = L"opengl_window_class",
    };

    if (!RegisterClassExW(&wc))
    {
        LogWin32Error("RegisterClassEx failed");
    }
    else
    {
        int width = CW_USEDEFAULT;
        int height = CW_USEDEFAULT;

        DWORD exstyle = WS_EX_APPWINDOW;
        DWORD style = WS_OVERLAPPEDWINDOW;

        if (WINDOW_WIDTH && WINDOW_HEIGHT)
        {
            style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    
            RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
            if (!AdjustWindowRectEx(&rect, style, FALSE, exstyle))
            {
                LogWin32Error("AdjustWindowRectEx failed");
                style = WS_OVERLAPPEDWINDOW;
            }
            else
            {
                width = rect.right - rect.left;
                height = rect.bottom - rect.top;
            }
        }

        struct Win32Context ctx;
        HWND wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"OpenGL Window",
            style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL, NULL, wc.hInstance, &ctx);
        if (!wnd)
        {
            LogWin32Error("CreateWindow failed");
        }
        else
        {
            for (;;)
            {
                MSG msg;
                if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                    {
                        break;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                    continue;
                }

                RenderFrame();

                if (!SwapBuffers(ctx.dc))
                {
                    LogWin32Error("SwapBuffers failed");
                }
            }
        }

        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }

    return 0;
}