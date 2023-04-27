//surfaces
inline b32 WorldSurfaceStackEmpty         (world *World) { return (World->SurfaceNext == World->Surfaces); }
inline b32 WorldSurfaceStackFull          (world *World) { return (World->SurfaceNext == World->SurfaceOnePastLast); }
//materials
inline b32 WorldMaterialStackEmpty      (world *World) { return (World->MaterialNext == World->Materials); }
inline b32 WorldMaterialStackFull       (world *World) { return (World->MaterialNext == World->MaterialOnePastLast); }
//textures
inline b32 WorldTextureStackEmpty      (world *World) { return (World->TextureNext == World->Textures); }
inline b32 WorldTextureStackFull       (world *World) { return (World->TextureNext == World->TextureOnePastLast); }

void WorldInit(world *World)
{
  world EmptyWorld = {0}; WriteToRef(World, EmptyWorld);
  World->Arena = ArenaInit(NULL, Kilobytes(1000), OSMemoryAlloc(Kilobytes(1000)));
  //surface
  World->SurfaceCount       = 0;
  World->SurfaceNext        = World->Surfaces;
  World->SurfaceOnePastLast = World->Surfaces+WORLD_STACK_MAXCOUNT;
  //materials
  World->MaterialCount       = 0;
  World->MaterialNext        = World->Materials;
  World->MaterialOnePastLast = World->Materials+WORLD_STACK_MAXCOUNT;
  //textures
  World->TextureCount       = 0;
  World->TextureNext        = World->Textures;
  World->TextureOnePastLast = World->Textures+WORLD_STACK_MAXCOUNT;
  
  //defaults
  World->DefaultTexId = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(1.0, 0.0, 1.0)); //default texture
  World->DefaultMatId = WorldMaterialAdd(World, MaterialKind_Lambert, World->DefaultTexId, 0.0, 0.0); //default material
  return;
}
void WorldSurfaceAdd(world *World, surface_kind Kind, void *SurfaceData)
{
  if(WorldSurfaceStackFull(World)) { return; }
  surface NewSurface = { .Kind = Kind };
  switch(Kind)
  {
    case SurfaceKind_Sphere: {
      NewSurface.Sphere = RefToInst(sphere, SurfaceData);
    } break;
    case SurfaceKind_SphereMoving: {
      NewSurface.SphereMoving = RefToInst(sphere_moving, SurfaceData);
    } break;
    case SurfaceKind_Plane: {
      NewSurface.Plane = RefToInst(plane, SurfaceData);
    } break;
    case SurfaceKind_BVHNode: { Assert(!"Invalid Codepath"); } break;
  }
  WriteToRef(World->SurfaceNext, NewSurface);
  World->SurfaceNext++;
  World->SurfaceCount++;
  return;
}
u32 WorldTextureAdd(world *World, texture_kind Kind, v3f64 Color)
{
  if(WorldTextureStackFull(World)) { return TEXTURE_INVALID_ID; }
  texture NewTex = {
    .Kind = Kind,
    .Color = Color,
  };
  WriteToRef(World->TextureNext, NewTex);
  // NOTE(MIGUEL): This could be an issue with overflow and computiong wrong index
  u32 TexId = (u32)((u64)World->TextureNext-(u64)World->Textures)/sizeof(texture);
  Assert(TexId != TEXTURE_INVALID_ID);
  World->TextureNext++;
  return TexId;
}
texture *WorldTextureGetFromId(world *World, u32 TexId)
{
  TexId = (TexId==TEXTURE_INVALID_ID)?World->DefaultTexId:TexId;
  texture *Tex = World->Textures + TexId;
  u64 Start   = (u64)World->Textures;
  u64 End     = (u64)World->TextureNext;
  u64 Address = (u64)Tex;
  if(!(Start<Address && Address<End)) { return &World->Textures[World->DefaultTexId]; }
  return Tex;
}
u32 WorldMaterialAdd(world *World, material_kind Kind, u32 TexId, f64 Fuzziness, f64 IndexOfRefraction)
{
  if(WorldMaterialStackFull(World)) { return MATERIAL_INVALID_ID; }
  material NewMat = { 
    .Kind = Kind,
    .TexId = TexId,
    .Fuzz = Clamp(Fuzziness, 0.0, 1.0),
    .IndexOfRefraction = IndexOfRefraction,
  };
  WriteToRef(World->MaterialNext, NewMat);
  // NOTE(MIGUEL): This could be an issue with overflow and computiong wrong index
  u32 MatId = (u32)((u64)World->MaterialNext-(u64)World->Materials)/sizeof(material);
  Assert(MatId != MATERIAL_INVALID_ID);
  World->MaterialNext++;
  return MatId;
}
material *WorldMaterialGetFromId(world *World, u32 MatId)
{
  MatId = (MatId == MATERIAL_INVALID_ID)?World->DefaultMatId:MatId;
  material *Mat = World->Materials + MatId;
  u64 Start   = (u64)World->Materials;
  u64 End     = (u64)World->MaterialNext;
  u64 Address = (u64)Mat;
  if(!(Start<Address && Address<End)) { return &World->Materials[0]; }
  return Mat;
}
b32 WorldHit(world *World, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  b32 SurfaceWasHit = 0;
  f64 ClosestHit = Maxt;
  hit TempHit = {0};
  
  for(surface *Surface=World->Surfaces; Surface!=World->SurfaceNext; Surface++)
  {
    if(SurfaceHit(Surface, &TempHit, Ray, Mint, ClosestHit))
    {
      SurfaceWasHit = 1;
      ClosestHit = TempHit.t;
      WriteToRef(Hit, TempHit);
    }
  }
  return SurfaceWasHit;
}
b32 WorldHitBVH(world *World, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  b32 SurfaceWasHit = 0;
  f64 ClosestHit = Maxt;
  hit TempHit = {0};
  
  if(SurfaceHit(&World->BVHRoot, &TempHit, Ray, Mint, ClosestHit))
  {
    SurfaceWasHit = 1;
    ClosestHit = TempHit.t;
    WriteToRef(Hit, TempHit);
  }
  return SurfaceWasHit;
}