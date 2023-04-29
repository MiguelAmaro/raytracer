#ifndef RTTEXTURE_H
#define RTTEXTURE_H

typedef enum texture_kind texture_kind;
enum texture_kind
{
  TextureKind_SolidColor,
  TextureKind_Checker,
  TextureKind_Noise,
  TextureKind_Image,
};
typedef struct texture texture;
struct texture
{
  texture_kind Kind;
  
  // solid color
  v3f64 Color;
  // checker
  texture *CheckerTex[2];
  // noise 
  noise *Perlin;
  f64 Scale;
  // image
  u8 *Data;
  u32 Width;
  u32 Height;
  u32 Stride; //bytes per pixel
  u32 Pitch;  //width*stride
};

v3f64 TextureGetColor(texture *Texture, f64 u, f64 v, v3f64 Pos)
{
  v3f64 Result = V3f64(0.0,0.0,0.0);
  switch(Texture->Kind)
  {
    case TextureKind_Checker: {
      f64 Sines = Sin(10.0*Pos.x)*Sin(10.0*Pos.y)*Sin(10.0*Pos.z);
      Result = ((Sines<0)?
                TextureGetColor(Texture->CheckerTex[1], u, v, Pos):
                TextureGetColor(Texture->CheckerTex[0], u, v, Pos));
    }break;
    case TextureKind_SolidColor: {
      Result = Texture->Color;
    }break;
    case TextureKind_Noise: {
      f64 Scalar = (0.5*
                    (1.0 + Sin(Texture->Scale*Pos.z + 10.0*NoiseGetValueTurbulance(Texture->Perlin, Pos, 7))));
      Result = Scale(V3f64(1.0,1.0,1.0), Scalar);
    }break;
    case TextureKind_Image: {
      if(Texture->Data == NULL) { Result = V3f64(1.0,0.0,1.0); }
      else{
        u =       Clamp(u,0.0,1.0);
        v = 1.0 - Clamp(v,0.0,1.0);
        u32 i = (s32)(u*(f64)Texture->Width);
        u32 j = (s32)(v*(f64)Texture->Height);
        if(i >= Texture->Width ) i = Texture->Width -1;
        if(j >= Texture->Height) j = Texture->Height-1;
        f64 ColorScale = 1.0/255.0;
        u8 *Pixel = Texture->Data + (j*Texture->Pitch) + (i*Texture->Stride);
        Result = V3f64(ColorScale*Pixel[0], 
                       ColorScale*Pixel[1],
                       ColorScale*Pixel[2]);
      }
    }break;
  }
  return Result;
}

#endif //RTTEXTURE_H
