#ifndef RTWORLD_H
#define RTWORLD_H

typedef enum light_kind light_kind;
enum light_kind
{
  LightKind_Point,
  LightKind_COUNT,
};
typedef struct light light;
struct light
{
  light_kind Kind;
  surface *Surface;
};


#define WORLD_STACK_MAXCOUNT (1024*4)
#define WORLD_STACK_NOISE_MAXCOUNT (16)
typedef struct world world;
struct world
{
  surface *BVHRoots;
  u32      BVHRootCount;
  
  
  surface   Surfaces [WORLD_STACK_MAXCOUNT];
  surface   SurfacesStatic[WORLD_STACK_MAXCOUNT]; //this is a hack. no sorting. bvh objs in Surfaces refence shit in here
  material  Materials[WORLD_STACK_MAXCOUNT];
  texture   Textures [WORLD_STACK_MAXCOUNT];
  light     Lights   [WORLD_STACK_MAXCOUNT];
  noise     Noises   [WORLD_STACK_NOISE_MAXCOUNT];
  
  surface  *SurfaceNext;        surface *SurfaceOnePastLast;
  surface  *SurfaceStaticNext;  surface *SurfaceStaticOnePastLast; //this is a hack
  material *MaterialNext;      material *MaterialOnePastLast;
  light    *LightNext;            light *LightOnePastLast;
  texture  *TextureNext;        texture *TextureOnePastLast;
  noise    *NoiseNext;            noise *NoiseOnePastLast; 
  
  u32 SurfaceCount;
  u32 SurfaceStaticCount;
  u32 MaterialCount;
  u32 TextureCount;
  u32 LightCount;
  u32 NoiseCount;
  
  v3f64 DefaultBackground[2];
  u32 DefaultNoiseId;
  u32 DefaultTexId;
  u32 DefaultMatId;
  arena Arena;
};

#define MATERIAL_INVALID_ID (0xffffffff)
#define TEXTURE_INVALID_ID (0xffffffff)
#define NOISE_INVALID_ID (0xffffffff)
#define LIGHT_INVALID_ID (0xffffffff)

world   *WorldInit(v3f64 Background);
surface *WorldHit(world *World, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt, b32 UseBVH);
u32      WorldMaterialAdd(world *World, material_kind Kind, u32 TexId, f64 Fuzziness, f64 IndexOfRefraction);
u32      WorldTextureAdd(world *World, texture_kind Kind, v3f64 Color, u32 CheckerTexIdA, u32 CheckerTexIdB, u32 NoiseId, f64 NoiseScale, const char *Path);
surface  *WorldSurfaceStaticAdd(world *World, surface_kind Kind, void *SurfaceData);
u32       WorldNoiseAdd(world *World, noise_kind Kind);
material *WorldMaterialGetFromId(world *World, u32 MatId);
texture  *WorldTextureGetFromId(world *World, u32 TexId);
noise    *WorldNoiseGetFromId(world *World, u32 NoiseId);
#endif //RTWORLD_H
