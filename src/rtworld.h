#ifndef RTWORLD_H
#define RTWORLD_H

#define WORLD_STACK_MAXCOUNT (1024)
#define WORLD_STACK_NOISE_MAXCOUNT (16)
typedef struct world world;
struct world
{
  surface BVHRoot;
  
  surface   Surfaces [WORLD_STACK_MAXCOUNT];
  material  Materials[WORLD_STACK_MAXCOUNT];
  texture   Textures [WORLD_STACK_MAXCOUNT];
  noise     Noises   [WORLD_STACK_NOISE_MAXCOUNT];
  
  surface  *SurfaceNext;  surface *SurfaceOnePastLast;
  material *MaterialNext; material *MaterialOnePastLast;
  texture  *TextureNext;  texture *TextureOnePastLast;
  noise    *NoiseNext;     noise *NoiseOnePastLast; 
  
  u32 SurfaceCount;
  u32 MaterialCount;
  u32 TextureCount;
  u32 NoiseCount;
  
  u32 DefaultNoiseId;
  u32 DefaultTexId;
  u32 DefaultMatId;
  arena Arena;
};

#define MATERIAL_INVALID_ID (0xffffffff)
#define TEXTURE_INVALID_ID (0xffffffff)
#define NOISE_INVALID_ID (0xffffffff)

void WorldInit(world *World);
b32  WorldHit(world *World, hit *Hit, ray Ray, f64 Mint, f64 Maxt);
b32  WorldHitBVH(world *World, hit *Hit, ray Ray, f64 Mint, f64 Maxt);
u32  WorldMaterialAdd(world *World, material_kind Kind, u32 TexId, f64 Fuzziness, f64 IndexOfRefraction);
u32   WorldTextureAdd(world *World, texture_kind Kind, v3f64 Color, u32 TexIdA, u32 TexIdB, u32 NoiseId, f64 NoiseScale, const char *Path);
u32     WorldNoiseAdd(world *World, noise_kind Kind);
material *WorldMaterialGetFromId(world *World, u32 MatId);
texture   *WorldTextureGetFromId(world *World, u32 TexId);
noise       *WorldNoiseGetFromId(world *World, u32 MatId);
#endif //RTWORLD_H
