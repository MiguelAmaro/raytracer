#ifndef RTUTIL_H
#define RTUTIL_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

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
f64 TrilinearInterp(v3f64 c[2][2][2], f64 u, f64 v, f64 w)
{
  f64 uu = u*u*(3-2*u);
  f64 vv = v*v*(3-2*v);
  f64 ww = w*w*(3-2*w);
  f64 Result = 0.0;
  for(u32 i=0;i<2;i++) {
    for(u32 j=0;j<2;j++) {
      for(u32 k=0;k<2;k++) {
        v3f64 weight_v = V3f64(u-i, v-j, w-k);
        Result += ((i*uu + (1-i)*(1-uu))*
                   (j*vv + (1-j)*(1-vv))*
                   (k*ww + (1-k)*(1-ww))*
                   Dot(c[i][j][k], weight_v));
      }}}
  return Result;
}
b32 NearZero(v3f64 a)
{
  f64 s = 1e-8;
  return ((Abs(a.x)<s) && (Abs(a.y)<s) && (Abs(a.z)<s));
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
u8 *ImageLoadFromFile(const char *Path, u32 *Width, u32 *Height, u32 *BytesPerPixel, u32 PixelCompCount, arena *Arena)
{
  u8 *Data = stbi_load(Path, (int *)Width, (int *)Height, (int *)BytesPerPixel, PixelCompCount);
  if(!Data) { fprintf(stderr, "could not load image"); WriteToRef(Width, 0); WriteToRef(Height, 0); }
  u32 Size = (*Width)*(*Height)*(*BytesPerPixel);
  u8 *Result = ArenaPushBlock(Arena, Size);
  MemoryCopy(Result, Size, Data, Size);
  stbi_image_free(Data);
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
void ExportAsPPM(const char *Path, u8 *ImageBuffer, s32 ImageWidth, s32 ImageHeight)
{
  FILE *outfile = fopen(Path, "w");
  fprintf(stderr, "\n\nexporting as ppm..\n");
  fprintf(outfile, "P3\n%d %d\n255\n", ImageWidth, ImageHeight);
  for(s32 y = ImageHeight-1; y>=0; --y)
  {
    fprintf(stderr, "\rscan lines remaining... %d", y);
    fflush(stderr);
    for(s32 x = 0; x<ImageWidth; ++x)
    {
      u32 *Pixel = ImageGetPixel(ImageBuffer, ImageWidth, x,y);
      u32 Color  = *Pixel;
      fprintf(outfile, "%3d %3d %3d  \n",
              (u32)(0xff&(Color>>24)),
              (u32)(0xff&(Color>>16)),
              (u32)(0xff&(Color>>8 )));
    }
    fprintf(outfile,"#newline\n");
  }
  fprintf(stderr, "\nexport finished..\n");
  fclose(outfile);
  return;
}
void ExportAsPNG(const char *Path, u8 *ImageBuffer, s32 ImageWidth, s32 ImageHeight)
{
  fprintf(stderr, "\n\nexporting as png....\n");
  u8 *CorrectedImageBuffer = OSMemoryAlloc(ImageWidth*ImageHeight*sizeof(u32));
  s32 PixelCount = ImageWidth*ImageHeight;
  u32 *Dest = (u32 *)CorrectedImageBuffer;
  u32 *Src  = (u32 *)ImageBuffer;
  for(s32 p=0;p<PixelCount;p++)
  {
    s32 x0 = p%ImageWidth;
    s32 y0 = (p/ImageWidth);
    s32 x1 = x0;
    s32 y1 = (ImageHeight-1)-y0;
    u32 Pixel = Src[y1*ImageWidth + x1];
    u8 a = (u8)(Pixel>>0)&0xff;
    u8 b = (u8)(Pixel>>8)&0xff;
    u8 g = (u8)(Pixel>>16)&0xff;
    u8 r = (u8)(Pixel>>24)&0xff;
    Pixel = ((b<<16)|(g<<8)|(r<<0)|(a<<24));
    Dest[y0*ImageWidth + x0] = Pixel;
  }
  
  stbi_write_png(Path, ImageWidth, ImageHeight, 4, CorrectedImageBuffer, 4*ImageWidth);
  fprintf(stderr, "\nexport finished..\n");
  return;
}


#endif //RTUTIL_H
