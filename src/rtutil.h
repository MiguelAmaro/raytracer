#ifndef RTUTIL_H
#define RTUTIL_H
u32 RandInt(u32 min, u32 max)
{
  u32 r = (u32)(RandF64Range(min, max+1));
  return r;
}
v3f64 RandInUnitSphere(void)
{
  while(1)
  {
    v3f64 Pos = V3f64RandBi();
    if(LengthSquared(Pos)>=1) continue;
    return Pos;
  }
}
v3f64 RandInHemisphere(v3f64 Normal)
{
  v3f64 UnitSphereVec = RandInUnitSphere();
  if(Dot(UnitSphereVec, Normal)>0.0) { return UnitSphereVec; }
  else                               { return Scale(UnitSphereVec, -1.0); }
}
v3f64 RandInUnitDisk(void)
{
  while(1)
  {
    v3f64 Pos = V3f64(RandF64Bi(), RandF64Bi(), 0.0);
    if(LengthSquared(Pos)>=1) continue;
    return Pos;
  }
}
b32 NearZero(v3f64 a)
{
  f64 s = 1e-8;
  return ((fabs(a.x)<s) && (fabs(a.y)<s) && (fabs(a.z)<s));
}
v3f64 RandUnitVector(void)
{
  return Normalize(RandInUnitSphere());
}
u32 *ImageGetPixel(u8 *ImageBuffer,  u32 ImageWidth, u32 x, u32 y)
{
  u32 *Pixels = (u32 *)ImageBuffer;
  u32 *Result = Pixels + ImageWidth*y + x;
  return Result;
}
void WriteColor(u32 *Pixel, v3f64 Color, s32 SamplesPerPixel)
{
  Color = SquareRoot(Scale(Color, 1.0/(f64)SamplesPerPixel));
  
  u8 r = (u8)(255.999*Clamp(Color.r, 0.0, 0.999));
  u8 g = (u8)(255.999*Clamp(Color.g, 0.0, 0.999));
  u8 b = (u8)(255.999*Clamp(Color.b, 0.0, 0.999));
  WriteToRef(Pixel, ((r << 24) | (g << 16) | (b << 8) | (0xff << 0)));
  return;
}
void WriteColorPPM(v3f64 Color, s32 SamplesPerPixel)
{
  Color = SquareRoot(Scale(Color, 1.0/(f64)SamplesPerPixel));
  fprintf(stdout, "%3d %3d %3d\n",
          (s32)(255.999*Clamp(Color.r, 0.0, 0.999)),
          (s32)(255.999*Clamp(Color.g, 0.0, 0.999)),
          (s32)(255.999*Clamp(Color.b, 0.0, 0.999)));
  return;
}
void WriteToPPM(u32 Color)
{
  fprintf(stdout, "%3d %3d %3d  \n",
          (u32)(0xff&(Color>>24)),
          (u32)(0xff&(Color>>16)),
          (u32)(0xff&(Color>>8 )));
  return;
}

#endif //RTUTIL_H
