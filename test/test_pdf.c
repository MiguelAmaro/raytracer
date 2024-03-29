#include "stdio.h"
#include "stdint.h"
#include <math.h>
// headers
#include "rttypes.h"
#include "rtatomics.h"
#include "rtos.h"
#include "rtmath.h"
#include "rtmemory.h"
#include "rtutil.h"
#include "rtray.h"
#include "rtnoise.h"
#include "rtcamera.h"
#include "rttexture.h"
#include "rtsurface.h"
#include "rtintegrator.h"
#include "rtworld.h"
#include "rtbvh.h"
#include "rtwork.h"
// source
#include "rtmemory.c"
#include "rtray.c"
#include "rtsurface.c"
#include "rtworld.c"
#include "rtprofile.h"

// testsuite
#include "rttest.c"


int main(void)
{
  v3f64 RayW = {0};
  pdf Pdf = {
    .Kind  = PdfKind_Cosine,
    .Basis = (m3f64){0},
    .Origin = (v3f64){1.0f, 0.0, 0.0f},
    .Surface = (surface *)NULL,
    .MixPdf[0] = NULL,
    .MixPdf[1] = NULL,
  };
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  RTTEST(V3f64(0,0,0), V3f64(0,0,0), "");
  
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  RTTEST(PdfCosineGetValue(&Pdf, RayW), 30.0, "");
  return;
}