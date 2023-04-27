#ifndef RTWORLD_H
#define RTWORLD_H

#define WORLD_STACK_MAXCOUNT (1024)
typedef struct world world;
struct world
{
  surface BVHRoot;
  
  surface   Surfaces [WORLD_STACK_MAXCOUNT];
  material  Materials[WORLD_STACK_MAXCOUNT];
  texture  Textures[WORLD_STACK_MAXCOUNT];
  
  surface  *SurfaceNext;   surface *SurfaceOnePastLast; u32 SurfaceCount;
  material *MaterialNext; material *MaterialOnePastLast; u32 MaterialCount;
  texture  *TextureNext; texture *TextureOnePastLast; u32 TextureCount;
  
  u32 DefaultTexId;
  u32 DefaultMatId;
  arena Arena;
};

#define MATERIAL_INVALID_ID (0xffffffff)
#define TEXTURE_INVALID_ID (0xffffffff)

void WorldInit(world *World);
b32  WorldHit(world *World, hit *Hit, ray Ray, f64 Mint, f64 Maxt);
b32  WorldHitBVH(world *World, hit *Hit, ray Ray, f64 Mint, f64 Maxt);
u32  WorldMaterialAdd(world *World, material_kind Kind, u32 TexId, f64 Fuzziness, f64 IndexOfRefraction);
u32  WorldTextureAdd(world *World, texture_kind Kind, v3f64 Color);
material *WorldMaterialGetFromId(world *World, u32 MatId);
texture  *WorldTextureGetFromId(world *World, u32 TexId);
#endif //RTWORLD_H
