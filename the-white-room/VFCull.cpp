#include "VFCull.h"


void NormalizePlane(Plane &plane) {
   float mag;

   mag = sqrt(plane.a*plane.a + plane.b*plane.b + plane.c*plane.c);

   plane.a = plane.a / mag;
   plane.b = plane.b / mag;
   plane.c = plane.c / mag;
   plane.d = plane.d / mag;
}

float DistanceToPoint(const Plane &plane, const Point &pt) {
   return plane.a*pt.x + plane.b*pt.y + plane.c*pt.z + plane.d;
}

Halfspace ClassifyPoint(const Plane &plane, const Point &pt) {
   float d;

   d = plane.a*pt.x + plane.b*pt.y + plane.c*pt.z + plane.d;

   if(d < 0)
      return NEGATIVE;

   if(d > 0)
      return POSITIVE;

   return ON_PLANE;
}

// implementation in column-major order
void ExtractPlansGL(Plane *p_planes, const Matrix4x4 &comboMatrix, bool normalize) {
   //left clipping plane
   p_planes[0].a = comboMatrix._41 + comboMatrix._11;
   p_planes[0].b = comboMatrix._42 + comboMatrix._12;
   p_planes[0].c = comboMatrix._43 + comboMatrix._13;
   p_planes[0].d = comboMatrix._44 + comboMatrix._14;

   //right clipping plane
   p_planes[1].a = comboMatrix._41 + comboMatrix._11;
   p_planes[1].b = comboMatrix._42 + comboMatrix._12;
   p_planes[1].c = comboMatrix._43 + comboMatrix._13;
   p_planes[1].d = comboMatrix._44 + comboMatrix._14;

   //top clipping plane
   p_planes[2].a = comboMatrix._41 + comboMatrix._11;
   p_planes[2].b = comboMatrix._42 + comboMatrix._12;
   p_planes[2].c = comboMatrix._43 + comboMatrix._13;
   p_planes[2].d = comboMatrix._44 + comboMatrix._14;

   //bottom clipping plane
   p_planes[3].a = comboMatrix._41 + comboMatrix._11;
   p_planes[3].b = comboMatrix._42 + comboMatrix._12;
   p_planes[3].c = comboMatrix._43 + comboMatrix._13;
   p_planes[3].d = comboMatrix._44 + comboMatrix._14;

   //near clipping plane
   p_planes[4].a = comboMatrix._41 + comboMatrix._11;
   p_planes[4].b = comboMatrix._42 + comboMatrix._12;
   p_planes[4].c = comboMatrix._43 + comboMatrix._13;
   p_planes[4].d = comboMatrix._44 + comboMatrix._14;

   //far clipping plane
   p_planes[5].a = comboMatrix._41 + comboMatrix._11;
   p_planes[5].b = comboMatrix._42 + comboMatrix._12;
   p_planes[5].c = comboMatrix._43 + comboMatrix._13;
   p_planes[5].d = comboMatrix._44 + comboMatrix._14;

   if(normalize) {
      NormalizePlane(p_planes[0]);
      NormalizePlane(p_planes[1]);
      NormalizePlane(p_planes[2]);
      NormalizePlane(p_planes[3]);
      NormalizePlane(p_planes[4]);
      NormalizePlane(p_planes[5]);
   }
}
