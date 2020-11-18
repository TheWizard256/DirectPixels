/*
 *  This is the Direct Pixels main header file.
 *  Direct Pixels is an open source API for simply opening a
 *  window and putting pixels directly onto it with the concept of
 *  the actual window and a buffer.
 *
 *  Created by Joseph Bingheim "The Wizard".
 *
 *  File created: 11/6/2020
 *  Last modified: 11/18/2020
 *
 *  Project uploaded to GitHub on 11/18/2020
 *  "Version 0.0.1"
 *
 *  See bottom of file to see updates.
 */

#ifndef _DIRECT_PIXELS_H_
#define _DIRECT_PIXELS_H_

#include <stdint.h>

/* Handles */
typedef struct dpWindowStruct dpWindow;
typedef struct dpBufferStruct dpBuffer;
typedef struct dpPixelStruct {
    union {
        struct {
            /*
             *  Yes the order does not make sense at all, here is an explanation,
             *  Usual format is R, G, B, and A. As you can see it is actually
             *  B, G, R, A.
             *  First, things have to be reversed if I want the uniform for hex to even
             *  work, so that part will remain.
             *  However, A should be first then, but it's not. That is because
             *  Win32 wants to be special and have the first eight bits to be buffer/padding
             *  instead of the last eight like a sane API.
             *  This completely screws up the the hex in the first place, so
             *  for now this will have to do, and until I start to work on the linux
             *  implementation we will have to see this garbage mess for now.
             *  I have made functions to make creating these structures easier.
             */
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        };
        uint32_t hex;
    };
} dpPixel;


/* Window functions */
dpWindow *dpwin_create(const char *, const uint32_t, const uint32_t);

void dpwin_tick(dpWindow *);
void dpwin_putBuffer(dpWindow *, dpBuffer *);

void dpwin_setSize(dpWindow *, const uint32_t, const uint32_t);

int32_t dpwin_isOpen(dpWindow *);
void dpwin_getSize(dpWindow *, uint32_t *, uint32_t *);
void dpwin_getMouseCoords(dpWindow *, int32_t *, int32_t *);
int32_t dpwin_getKey(dpWindow *, const int32_t);
int32_t dpwin_getButton(dpWindow *, const int32_t);

void dpwin_destroy(dpWindow *);

/* Pixel functions */
dpPixel dppix_hex(const uint32_t);
dpPixel dppix_rgb(const uint8_t, const uint8_t, const uint8_t);
dpPixel dppix_rgba(const uint8_t, const uint8_t, const uint8_t, const uint8_t);

/* Buffer functions */
dpBuffer *dpbuf_create(const uint32_t, const uint32_t);

void dpbuf_clear(dpBuffer *);

void dpbuf_putPixel(dpBuffer *, const int32_t, const int32_t, const dpPixel);
void dpbuf_putPixel3(dpBuffer *, const int32_t, const int32_t, const uint8_t, const uint8_t, const uint8_t);
void dpbuf_putPixel4(dpBuffer *, const int32_t, const int32_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t);

void dpbuf_setClearColor(dpBuffer *, const dpPixel);

dpPixel *dpbuf_getPixelPointer(dpBuffer *);

void dpbuf_destroy(dpBuffer *);


/* Math section */

typedef struct dpVec2Struct {
    float x;
    float y;
} dpVec2;
typedef struct dpVec3Struct {
    float x;
    float y;
    float z;
} dpVec3;
typedef struct dpMat3Struct {
    float e[9];
} dpMat3;

dpVec2 dpvec2_add(const dpVec2, const dpVec2);
dpVec2 dpvec2_sub(const dpVec2, const dpVec2);
dpVec2 dpvec2_mult(const dpVec2, const dpVec2);
dpVec2 dpvec2_div(const dpVec2, const dpVec2);
float dpvec2_dot(const dpVec2, const dpVec2);
dpVec2 dpvec2_cross(const dpVec2, const dpVec2);
float dpvec2_magnitude(const dpVec2);
dpVec2 dpvec2_normalize(const dpVec2);

dpVec3 dpvec3_add(const dpVec3, const dpVec3);
dpVec3 dpvec3_sub(const dpVec3, const dpVec3);
dpVec3 dpvec3_mult(const dpVec3, const dpVec3);
dpVec3 dpvec3_div(const dpVec3, const dpVec3);
float dpvec3_dot(const dpVec3, const dpVec3);
dpVec3 dpvec3_cross(const dpVec3, const dpVec3);
float dpvec3_magnitude(const dpVec3);
dpVec3 dpvec3_normalize(const dpVec3);

dpMat3 dpmat3_identity();
dpMat3 dpmat3_translate(const float, const float);
dpMat3 dpmat3_translatev(const dpVec2);
dpMat3 dpmat3_scale(const float, const float);
dpMat3 dpmat3_scalev(const dpVec2);
dpMat3 dpmat3_rotate(const float);
dpMat3 dpmat3_add(const dpMat3, const dpMat3);
dpMat3 dpmat3_sub(const dpMat3, const dpMat3);
dpMat3 dpmat3_mult(const dpMat3, const dpMat3);

/* Drawing funcion section */
/* Not yet added */


#endif /* End file */

/*
 *  Notes 11/7/2020 (Joseph):
 *      Added the header-mentioned structures; the window and the buffer. Currently, only some
 *      is added to the window and absolutely no functionality is added to the buffer. Just trying
 *      to get the window to work first. I am really hoping to somehow allow for multiple windows at a time.
 *      Also added a super neat pixel structure with a union. I used to hate and not understand unions, but
 *      now a I love them. Super niche though.
 *  Notes 11/13/2020 (Joseph):
 *      Added the soon beginnings of the advanced graphics. I planned to have simple matrix math and shape drawing
 *      helper functions to make some aspects of drawing things easier. I have added the declarations but not yet the
 *      definitions of rectangle drawing and transformation functions. Will add more drawing helper functions like
 *      polygon, ellipse, triangle, and line drawing. Before this though, I had to implement a small set of vector and matrix
 *      structs and functions, which I did. Most of those have the definitions as well.
 *      I also learned that a lot of what I am trying to do requires a definite amount of bits and the compiler specifications
 *      for different sized integers change. So today or sometime inbetween now and the next update I will swap out all 'int''s
 *      and 'unsigned int''s with their respective <stdint.h> counterpart so that way they are both garanteed 32 bits.
 *  Notes 11/17/2020 (Joseph):
 *      Removed the declarations for drawing simple shapes. Decided I want to optimize all code possible before I move on
 *      to the next part.
 *      Note to self: I need to add a little more functionality to the window.
 *      Majority of change is in the directpixels.c file. See that file for more changes.
 */
