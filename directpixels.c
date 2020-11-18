/*
 *  This is the direct Pixels main source file.
 *  Look at "directpixels.h"
 *
 *  Created by Joseph Bingheim "The Wizard".
 *
 *  File created: 11/6/2020
 *  Last modified: 11/18/2020
 *
 *  See bottom of file to see updates.
 */
//#define DP_BUILD_WINDOWS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "platformtest.h"

#if defined(DP_BUILD_WINDOWS)
#include <windows.h>
#include <windowsx.h>
#elif defined(DP_BUILD_LINUX)
#endif

#include "directpixels.h"

#define DP_MAX_KEYS 512
#define DP_MAX_BUTTONS 16
#define DP_TITLE_LENGTH 128

#define ECHO(a) printf("-> Pos: %d <-\n", a);

#if defined(DP_BUILD_WINDOWS)
LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

/* Structure Definitions */

typedef struct dpWindowStruct {
    uint32_t width;
    uint32_t height;
    char title[DP_TITLE_LENGTH];
    int32_t keys[DP_MAX_KEYS];
    int32_t buttons[DP_MAX_BUTTONS];
    int32_t mousex;
    int32_t mousey;
    int32_t open;
    int32_t id;
#if defined(DP_BUILD_WINDOWS)
    WNDCLASS wc;
    HWND hwnd;
    HDC hdc;
#elif defined(DP_BUILD_LINUX)
#endif
} dpWindow;

typedef struct dpBufferStruct {
    uint32_t width;
    uint32_t height;
    uint32_t length;
    dpPixel clearcolor;
    dpPixel *pixels;
#if defined(DP_BUILD_WINDOWS)
    BITMAPINFO bitmapinfo;
#endif
} dpBuffer;


/* Window structure functions */

dpWindow *dpwin_create(const char *title, const uint32_t width, const uint32_t height) {
    uint32_t i;
    dpWindow *dpwin = malloc(sizeof(dpWindow));
    memset(dpwin->title, 0, sizeof(dpwin->title)); /* <- Junk characters cleared */
    for(i = 0; title[i]; i++)
        dpwin->title[i] = title[i]; /* <- Deep copy (stack) */
    dpwin->width = width;
    dpwin->height = height;
    dpwin->mousex = 0;
    dpwin->mousey = 0;
    dpwin->open = 0;
    memset(dpwin->keys, 0, DP_MAX_KEYS);
    memset(dpwin->buttons, 0, DP_MAX_BUTTONS);

#if defined(DP_BUILD_WINDOWS)
    HINSTANCE hInstance = GetModuleHandle(NULL);
    memset(&dpwin->wc, 0, sizeof(WNDCLASS));
    dpwin->wc.lpfnWndProc = WindProc;
    dpwin->wc.hInstance = hInstance;
    dpwin->wc.lpszClassName = dpwin->title;
    RegisterClass(&dpwin->wc);

    RECT rect = { 0, 0, dpwin->width, dpwin->height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    RECT centerrect;
    GetClientRect(GetDesktopWindow(), &centerrect);
    centerrect.left = (centerrect.right / 2) - ((rect.right - rect.left) / 2);
    centerrect.top = (centerrect.bottom / 2) - ((rect.bottom - rect.top) / 2);

    dpwin->hwnd = CreateWindowEx(
        0,
        dpwin->title, /* Classname/id */ /* TODO: Find a better way to organize class ID's */
        dpwin->title, /* Window title */
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        centerrect.left,
        centerrect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, /* Parent window, */
        NULL, /* Menu, */
        hInstance, /* Instance, */
        NULL /* App data */
    );
    if(dpwin->hwnd == NULL) {
        free(dpwin);
        return NULL;
    }

    dpwin->hdc = GetDC(dpwin->hwnd);
    SetWindowLongPtr(dpwin->hwnd, GWLP_USERDATA, (LONG_PTR)(dpwin));
    ShowWindow(dpwin->hwnd, SW_SHOW);
    dpwin->open = 1;
#elif defined(DP_BUILD_LINUX)
#endif
    return dpwin;
}

void dpwin_tick(dpWindow *dpwin) {
#if defined(DP_BUILD_WINDOWS)
    MSG msg = { 0 };
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        if(msg.message == WM_QUIT)
            dpwin->open = 0;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#elif defined(DP_BUILD_LINUX)
#endif
}

void dpwin_putBuffer(dpWindow *dpwin, dpBuffer *dpbuf) {
#if defined(DP_BUILD_WINDOWS)
    RECT rect;
    GetClientRect(dpwin->hwnd, &rect);
    SetStretchBltMode(dpwin->hdc, STRETCH_DELETESCANS);
    StretchDIBits(
        dpwin->hdc,
        0, rect.bottom,
        dpwin->width, -rect.bottom,
        0, 0,
        dpbuf->width, dpbuf->height,
        dpbuf->pixels,
        &dpbuf->bitmapinfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
#elif defined(DP_BUILD_LINUX)
#endif
}

void dpwin_setSize(dpWindow *dpwin, const uint32_t width, const uint32_t height) {
#if defined(DP_BUILD_WINDOWS)
#elif defined(DP_BUILD_LINUX)
#endif
}

int32_t dpwin_isOpen(dpWindow *dpwin) {
    return dpwin->open;
}

void dpwin_getSize(dpWindow *dpwin, uint32_t *width, uint32_t *height) {
    *width = dpwin->width;
    *height = dpwin->height;
}

void dpwin_getMouseCoords(dpWindow *dpwin, int32_t *mousex, int32_t *mousey) {
    *mousex = dpwin->mousex;
    *mousey = dpwin->mousey;
}

int32_t dpwin_getKey(dpWindow *dpwin, const int32_t keycode) {
    return dpwin->keys[keycode];
}

int32_t dpwin_getButton(dpWindow *dpwin, const int32_t buttoncode) {
    return dpwin->buttons[buttoncode];
}

void dpwin_destroy(dpWindow *dpwin) {
#if defined(DP_BUILD_WINDOWS)
    DeleteDC(dpwin->hdc);
    DestroyWindow(dpwin->hwnd);
#endif
    free(dpwin);
}

#if defined(DP_BUILD_WINDOWS)
LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    dpWindow *dpwin = (dpWindow *)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if(dpwin == NULL)
        return DefWindowProc(hwnd, msg, wParam, lParam);
    LRESULT result;
    switch(msg) {
        case WM_SIZING: {
        case WM_SIZE: {
            RECT rect;
            GetClientRect(dpwin->hwnd, &rect);
            dpwin->width = rect.right - rect.left;
            dpwin->height = rect.bottom - rect.top;
            result = 0;
            break;
        } }
        case WM_KEYDOWN:
            dpwin->keys[wParam] = 1;
            result = 0;
            break;
        case WM_KEYUP:
            dpwin->keys[wParam] = 0;
            result = 0;
            break;

        case WM_LBUTTONDOWN:
            dpwin->buttons[0] = 1;
            result = 0;
            break;
        case WM_LBUTTONUP:
            dpwin->buttons[0] = 0;
            result = 0;
            break;
        case WM_RBUTTONDOWN:
            dpwin->buttons[1] = 1;
            result = 0;
            break;
        case WM_RBUTTONUP:
            dpwin->buttons[1] = 0;
            result = 0;
            break;
        case WM_MOUSEMOVE:
            dpwin->mousex = GET_X_LPARAM(lParam);
            dpwin->mousey = GET_Y_LPARAM(lParam);
            result = 0;
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            DestroyWindow(hwnd);
            result = 0;
            break;
        case WM_QUIT:
            PostQuitMessage(0);
            result = 0;
        default:
            result = DefWindowProc(hwnd, msg, wParam, lParam);
            break;
    }
    return result;
}
#endif


/* Pixel structure function */

dpPixel dppix_hex(const uint32_t hex) {
    dpPixel dppix;
    dppix.hex = hex;
    return dppix;
}

dpPixel dppix_rgb(const uint8_t r, const uint8_t g, const uint8_t b) {
    dpPixel dppix;
    dppix.r = r;
    dppix.g = g;
    dppix.b = b;
    dppix.a = 0;
    return dppix;
}

dpPixel dppix_rgba(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
    dpPixel dppix;
    dppix.r = r;
    dppix.g = g;
    dppix.b = b;
    dppix.a = a;
    return dppix;
}


/* Buffer structure functions */

dpBuffer *dpbuf_create(const uint32_t width, const uint32_t height) {
    dpBuffer *dpbuf = malloc(sizeof(dpBuffer));
    dpbuf->width = width;
    dpbuf->height = height;
    dpbuf->length = dpbuf->width * dpbuf->height;
    dpbuf->clearcolor = dppix_hex(0x0);
    dpbuf->pixels = malloc(sizeof(dpPixel) * dpbuf->length);
    dpbuf_clear(dpbuf);
#if defined(DP_BUILD_WINDOWS)
    dpbuf->bitmapinfo.bmiHeader.biSize = sizeof(dpbuf->bitmapinfo.bmiHeader);
    dpbuf->bitmapinfo.bmiHeader.biWidth = dpbuf->width;
    dpbuf->bitmapinfo.bmiHeader.biHeight = dpbuf->height;
    dpbuf->bitmapinfo.bmiHeader.biPlanes = 1;
    dpbuf->bitmapinfo.bmiHeader.biBitCount = 32;
    dpbuf->bitmapinfo.bmiHeader.biCompression = BI_RGB;
#endif
    return dpbuf;
}

void dpbuf_clear(dpBuffer *dpbuf) {
    uint32_t i;
    for(i = 0; i < dpbuf->length; i++)
        dpbuf->pixels[i] = dpbuf->clearcolor;
}

void dpbuf_putPixel(dpBuffer *dpbuf, const int32_t x, const int32_t y, const dpPixel pixel) {
    if(x >= 0 && y >= 0 && x < dpbuf->width && y < dpbuf->height)
        dpbuf->pixels[y * dpbuf->width + x] = pixel;
}

void dpbuf_putPixel3(dpBuffer *dpbuf, const int32_t x, const int32_t y, const uint8_t r, const uint8_t g, const uint8_t b) {
    if(x >= 0 && y >= 0 && x < dpbuf->width && y < dpbuf->height)
        dpbuf->pixels[y * dpbuf->width + x] = dppix_rgb(r, g, b);
}

void dpbuf_putPixel4(dpBuffer *dpbuf, const int32_t x, const int32_t y, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
    if(x >= 0 && y >= 0 && x < dpbuf->width && y < dpbuf->height)
        dpbuf->pixels[y * dpbuf->width + x] = dppix_rgba(r, g, b, a);
}

void dpbuf_setClearColor(dpBuffer *dpbuf, const dpPixel pixel) {
    dpbuf->clearcolor = pixel;
}

dpPixel *dpbuf_getPixelPointer(dpBuffer *dpbuf) {
    return dpbuf->pixels;
}

void dpbuf_destroy(dpBuffer *dpbuf) {
    free(dpbuf->pixels);
    free(dpbuf);
}


/* Vector 2 fuctions */

dpVec2 dpvec2_add(const dpVec2 a, const dpVec2 b) {
    dpVec2 v = { a.x + b.x, a.y + b.y };
    return v;
}

dpVec2 dpvec2_sub(const dpVec2 a, const dpVec2 b) {
    dpVec2 v = { a.x - b.x, a.y - b.y };
    return v;
}

dpVec2 dpvec2_mult(const dpVec2 a, const dpVec2 b) {
    dpVec2 v = { a.x * b.x, a.y * b.y };
    return v;
}

dpVec2 dpvec2_div(const dpVec2 a, const dpVec2 b) {
    dpVec2 v = { a.x / b.x, a.y / b.y };
    return v;
}

float dpvec2_dot(const dpVec2 a, const dpVec2 b) {
    return a.x * b.x + a.y * b.y;
}

dpVec2 dpvec2_cross(const dpVec2 a, const dpVec2 b) {
    dpVec2 v = { 0.0f, 0.0f };
    return v;
}

float dpvec2_magnitude(const dpVec2 a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

dpVec2 dpvec2_normalize(const dpVec2 a) {
    float mag = sqrt(a.x * a.x + a.y * a.y);
    dpVec2 v = { a.x / mag, a.y / mag };
    return v;
}


/* Vector 3 functions */

dpVec3 dpvec3_add(const dpVec3 a, const dpVec3 b) {
    dpVec3 v = { a.x + b.x, a.y + b.y, a.z + b.z };
    return v;
}

dpVec3 dpvec3_sub(const dpVec3 a, const dpVec3 b) {
    dpVec3 v = { a.x - b.x, a.y - b.y, a.z - b.z };
    return v;
}

dpVec3 dpvec3_mult(const dpVec3 a, const dpVec3 b) {
    dpVec3 v = { a.x * b.x, a.y * b.y, a.z * b.z };
    return v;
}

dpVec3 dpvec3_div(const dpVec3 a, const dpVec3 b) {
    dpVec3 v = { a.x / b.x, a.y / b.y, a.z / b.z };
    return v;
}

float dpvec3_dot(const dpVec3 a, const dpVec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

dpVec3 dpvec3_cross(const dpVec3 a, const dpVec3 b) {
    dpVec3 v = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x - b.z,
        a.x * b.y - a.y - b.x
    };
    return v;
}

float dpvec3_magnitude(const dpVec3 a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

dpVec3 dpvec3_normalize(const dpVec3 a) {
    float mag = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    dpVec3 v = { a.x / mag, a.y / mag, a.z / mag };
    return v;
}

dpMat3 dpmat3_identity() {
    dpMat3 m = { { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f } };
    return m;
}

dpMat3 dpmat3_translate(const float tx, const float ty) {
    dpMat3 m = { { 1.0f, 0.0f,   tx, 0.0f, 1.0f,   ty, 0.0f, 0.0f, 1.0f } };
    return m;
}

dpMat3 dpmat3_translatev(const dpVec2 tr) {
    dpMat3 m = { { 1.0f, 0.0f, tr.x, 0.0f, 1.0f, tr.y, 0.0f, 0.0f, 1.0f } };
    return m;
}

dpMat3 dpmat3_scale(const float sx, const float sy) {
    dpMat3 m = { {   sx, 0.0f, 0.0f, 0.0f,   sy, 0.0f, 0.0f, 0.0f, 1.0f } };
    return m;
}

dpMat3 dpmat3_scalev(const dpVec2 sx) {
    dpMat3 m = { { sx.x, 0.0f, 0.0f, 0.0f, sx.y, 0.0f, 0.0f, 0.0f, 1.0f } };
    return m;
}

dpMat3 dpmat3_rotate(const float r) {
    dpMat3 m = { { cos(r), 0.0f, 0.0f, sin(r), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f } };
    m.e[1] = -m.e[3];
    m.e[4] = m.e[0];
    return m;
}

dpMat3 dpmat3_add(const dpMat3 a, const dpMat3 b) {
    dpMat3 m = { {
        a.e[0] + b.e[0],
        a.e[1] + b.e[1],
        a.e[2] + b.e[2],
        a.e[3] + b.e[3],
        a.e[4] + b.e[4],
        a.e[5] + b.e[5],
        a.e[6] + b.e[6],
        a.e[7] + b.e[7],
        a.e[8] + b.e[8],
    } };
    return m;
}

dpMat3 dpmat3_sub(const dpMat3 a, const dpMat3 b) {
    dpMat3 m = { {
        a.e[0] - b.e[0],
        a.e[1] - b.e[1],
        a.e[2] - b.e[2],
        a.e[3] - b.e[3],
        a.e[4] - b.e[4],
        a.e[5] - b.e[5],
        a.e[6] - b.e[6],
        a.e[7] - b.e[7],
        a.e[8] - b.e[8],
    } };
    return m;
}

dpMat3 dpmat3_mult(const dpMat3 a, const dpMat3 b) {
    dpMat3 m = { {
        a.e[0] * b.e[0] + a.e[1] * b.e[3] + a.e[2] * b.e[6],
        a.e[0] * b.e[1] + a.e[1] * b.e[4] + a.e[2] * b.e[7],
        a.e[0] * b.e[2] + a.e[1] * b.e[5] + a.e[2] * b.e[8],
        a.e[3] * b.e[0] + a.e[4] * b.e[3] + a.e[5] * b.e[6],
        a.e[3] * b.e[1] + a.e[4] * b.e[4] + a.e[5] * b.e[7],
        a.e[3] * b.e[2] + a.e[4] * b.e[5] + a.e[5] * b.e[8],
        a.e[6] * b.e[0] + a.e[7] * b.e[3] + a.e[8] * b.e[6],
        a.e[6] * b.e[1] + a.e[7] * b.e[4] + a.e[8] * b.e[7],
        a.e[6] * b.e[2] + a.e[7] * b.e[5] + a.e[8] * b.e[8]
    } };
    return m;
}


/* Drawing functions */

struct _gfxstruct_ {
    dpMat3 transform;
    dpMat3 translate;
    dpMat3 scale;
    dpMat3 rotate;
    dpPixel color;
} _gfx_;



/* End file */

/*  Notes 11/7/2020 (Joseph):
 *      This is actually the third or so time I attempted making this entire project.
 *      The first time I attempted it I linked it with OpenGL, but I wanted to go even
 *      lower level, and I didn't like using shaders for essentially 2D graphics. I also
 *      didn't want people who wanted to compile this not have to do any crazy additional
 *      linking or anything.
 *      The other attempts were the ways I am doing it now, which is using the default operating
 *      system libraries. I got stopped when I couldn't figure out a way to have multiple windows.
 *      In C++ this would be easy because I could put a function inside an actual class or even a
 *      struct, but I can't because this is C.
 *      I also like the primitive, low-res art style that low resolution games have, so I am going to make
 *      minimalistic math library as well as having the buffer structure I have already made scale up from
 *      a low resolution to what ever the actual window size is. Say the buffer size is 640x480 and the window
 *      size is 1920p, then the 640x480 will scale up accordingly giving that nice pixelated retro feel. Matrix
 *      maths for transformations will make this look really cool to, at least in my opinion.
 *  Notes 11/13/2020 (Joseph):
 *      I made huge progress compared to my last Win32 attempt of creating a window. Reorganized the pixel
 *      structure as well for now to make drawing to the window easier. Keyboard, mouse, and mouse button input
 *      works completely, just have to create some constants/defines for the keycodes. For now the windows defines
 *      will have to be used.
 *      I did have the drawing completely smooth and functional with different sized buffers like I wanted via UV mapping
 *      as well as having it work whenever the window got resized. However, with only two rectangles being drawn on
 *      the screen the time between each render was a whopping 300ms. After some optimization I got it down to 200ms but it
 *      still was not enough. The good news is that StretchDIBits actually resizes a buffer of pixels automatically and it cuts
 *      it down to 10ms. Very bad news however is that it actually is not working properly. The buggy-ness is very unpredictable
 *      and I can't tell what is going on. It should work if I am reading the documentation correctly.
 *      There is also really annoying glitch-iness with resizing the window and the area to draw not getting updated. Ever since this
 *      change I had to make my code look uglier for efficiency but now everything is breaking...
 *  Notes 11/17/2020 (Joseph):
 *      I finally fixed the issue where StretchDIBits would break everything upon resizing. The issue was that the
 *      Windows structure "BITMAPINFO" actually refers to the source bitmap, not the bitmap of the window. So in the code
 *      every time the window would resize I updated the BITMAPINFO's width and height to that of the window when I actually I
 *      should have left it the hell alone, and in addition, put it inside the dpBuffer structure because it turns out
 *      BITMAPINFO (at least in this case) has nothing to do with the window at all.
 *      Since this change everything has been working fantastically. The bare minimum of this project has is now complete
 *      and I can add the little bonus stuff now whenever I can.
 *      This would include the following:
 *          - "Drawing" (outlining) basic shapes and lines (...and polygons?)
 *          - "Drawing" (outlining) basic shapes and lines (and polygons?) and allow for transformations
 *          - "Filling" (outlining) basic shapes and lines (...and polygons?)
 *          - "Filling" (outlining) basic shapes and lines (and polygons?) and allow for transformations
 *          - Drawing textures
 *          - Drawing textures and allow for transforms
 *          - Drawing text
 *          - Drawing text and allow for transforms
 *          - For the two above... I guess I need to figure out how to use TTF files
 *      I already have Vector2, Vector3, and Matrix3 structures and functions to handle all the transformation related functions
 *      when the time comes.
 *      Everything listed above should be completely cross-platform as it is soley math operations and modifying an
 *      array of pixels. The pixels are of course the buffer, and these pixels will exist reguardless of platforms.
 *      Thus, when I go to make the Linux implementation it should be a matter of just getting the dpWindow and dpBuffer
 *      structures to work.
 *      The last thing I did was remove about ten lines of code because of some redundancy in the switch statement of my WindProc function.
 *      I am also really debating on whether or not to have more game related functionality in this project or not, like playing
 *      audio or having some RNG libraries. For now I am of course working on this graphical aspect, but I doubt this should be used for large
 *      games. If you wanted some actual speed people should rather use OpenGL, DirectX, or libraries based off those.
 *      I could (if I really wanted or had the time) add some GPU accelerated code that can be enabled as an option...
 *      For now however: OPTIMIZATION!
 */

