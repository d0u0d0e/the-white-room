/* 
 * File:   VFCull.h
 * Author: Ben
 *
 * Created on January 29, 2013, 9:29 PM
 */

#ifndef VFCULL_H
#define	VFCULL_H



#endif	/* VFCULL_H */

struct Plane {
   float a, b, c, d;
};

struct Point {
    float x, y, z, w;
};

// matrix for column-major order. (OpenGL)
struct Matrix4x4 {
   float _11, _21, _31, _41;
   float _12, _22, _32, _42;
   float _13, _23, _33, _43;
   float _14, _24, _34, _44;
};

/*
// matrix for row-major order. (Direct3D)
struct Matrix4x4 {
   float _11, _12, _13, _14;
   float _21, _22, _23, _24;
   float _31, _32, _33, _34;
   float _41, _42, _43, _44;
};
*/

enum Halfspace {
   NEGATIVE = -1,
   ON_PLANE = 0,
   POSITIVE = 1
};
