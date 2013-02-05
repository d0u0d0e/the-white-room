#include "VFCull.h"


int main() {
   Matrix4x4 *mvMatrix;  //clipping planes in world space
   Matrix4x4 *mvpMatrix; //clipping planes in object/model space
   Matrix4x4 *pMatrix;   // clipping planes in view space
   Plane p_planes[6];     //holds 6 Planes
   Point *pt;
   
   //load data into matrix
   mvMatrix._11 = ModelTrans.modelViewMatrix[0];
   mvMatrix._12 = ModelTrans.modelViewMatrix[1];
   mvMatrix._13 = ModelTrans.modelViewMatrix[2];
   mvMatrix._14 = ModelTrans.modelViewMatrix[3];
   
   mvMatrix._21 = ModelTrans.modelViewMatrix[4];
   mvMatrix._22 = ModelTrans.modelViewMatrix[5];
   mvMatrix._23 = ModelTrans.modelViewMatrix[6];
   mvMatrix._24 = ModelTrans.modelViewMatrix[7];
   
   mvMatrix._31 = ModelTrans.modelViewMatrix[8];
   mvMatrix._32 = ModelTrans.modelViewMatrix[9];
   mvMatrix._33 = ModelTrans.modelViewMatrix[10];
   mvMatrix._34 = ModelTrans.modelViewMatrix[11];
   
   mvMatrix._41 = ModelTrans.modelViewMatrix[12];
   mvMatrix._42 = ModelTrans.modelViewMatrix[13];
   mvMatrix._43 = ModelTrans.modelViewMatrix[14];
   mvMatrix._44 = ModelTrans.modelViewMatrix[15];
   
   ExtractPlanesGL(&p_planes, mvMatrix, true); 
   
   //pt = object's representative point
   if(Cull(p_planes, pt)) {   
       //don't draw
   }
   else {
       //draw
   }
}