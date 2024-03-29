//surfaces
inline b32 WorldSurfaceStackEmpty         (world *World) { return (World->SurfaceNext == World->Surfaces); }
inline b32 WorldSurfaceStackFull          (world *World) { return (World->SurfaceNext == World->SurfaceOnePastLast); }
//surfaces (hackk!!!)
inline b32 WorldSurfacesStaticStackEmpty         (world *World) { return (World->SurfaceStaticNext == World->SurfacesStatic); }
inline b32 WorldSurfacesStaticStackFull          (world *World) { return (World->SurfaceStaticNext == World->SurfaceStaticOnePastLast); }
//materials
inline b32 WorldMaterialStackEmpty      (world *World) { return (World->MaterialNext == World->Materials); }
inline b32 WorldMaterialStackFull       (world *World) { return (World->MaterialNext == World->MaterialOnePastLast); }
//textures
inline b32 WorldTextureStackEmpty      (world *World) { return (World->TextureNext == World->Textures); }
inline b32 WorldTextureStackFull       (world *World) { return (World->TextureNext == World->TextureOnePastLast); }
//lights
inline b32 WorldLightStackEmpty      (world *World) { return (World->LightNext == World->Lights); }
inline b32 WorldLightStackFull       (world *World) { return (World->LightNext == World->LightOnePastLast); }
//noises
inline b32 WorldNoiseStackEmpty      (world *World) { return (World->NoiseNext == World->Noises); }
inline b32 WorldNoiseStackFull       (world *World) { return (World->NoiseNext == World->NoiseOnePastLast); }

world *WorldInit(v3f64 Background)
{
  arena Arena = ArenaInit(NULL, Gigabytes(2), OSMemoryAlloc(Gigabytes(2)));
  world *World = ArenaPushType(&Arena, world);
  MemorySet(0, World, sizeof(world));
  //arena
  World->Arena = Arena;
  //surface
  World->SurfaceCount       = 0;
  World->SurfaceNext        = World->Surfaces;
  World->SurfaceOnePastLast = World->Surfaces+WORLD_STACK_MAXCOUNT;
  //surface (hack!!!)
  World->SurfaceStaticCount       = 0;
  World->SurfaceStaticNext        = World->SurfacesStatic;
  World->SurfaceStaticOnePastLast = World->SurfacesStatic+WORLD_STACK_MAXCOUNT;
  //materials
  World->MaterialCount       = 0;
  World->MaterialNext        = World->Materials;
  World->MaterialOnePastLast = World->Materials+WORLD_STACK_MAXCOUNT;
  //textures
  World->TextureCount       = 0;
  World->TextureNext        = World->Textures;
  World->TextureOnePastLast = World->Textures+WORLD_STACK_MAXCOUNT;
  //lights
  World->LightCount       = 0;
  World->LightNext        = World->Lights;
  World->LightOnePastLast = World->Lights+WORLD_STACK_MAXCOUNT;
  //noises
  World->NoiseCount       = 0;
  World->NoiseNext        = World->Noises;
  World->NoiseOnePastLast = World->Noises+WORLD_STACK_NOISE_MAXCOUNT;
  
  //defaults
  World->DefaultBackground[0] = Background;
  World->DefaultBackground[0] = Background;
  World->DefaultNoiseId = WorldNoiseAdd(World, NoiseKind_Perin);
  World->DefaultTexId = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(1.0, 0.0, 1.0),0,0,0,0,NULL); //default texture
  World->DefaultMatId = WorldMaterialAdd(World, MaterialKind_Lambert, World->DefaultTexId, 0.0, 0.0); //default material
  return World;
}
void WorldBVHRootListInit(world *World, u32 RootCount)
{
  World->BVHRoots = ArenaPushArray(&World->Arena, RootCount, surface);
  World->BVHRootCount = RootCount;
  return;
}

//~ APPENDERS
surface *WorldSurfaceAdd(world *World, surface_kind Kind, void *SurfaceData)
{
  if(WorldSurfaceStackFull(World)) { return NULL; }
  surface *Result = NULL;
  surface NewSurface = { .Kind = Kind };
  switch(Kind)
  {
    case SurfaceKind_Sphere: {
      NewSurface.Sphere = ObjCopyFromRef(sphere, SurfaceData);
    } break;
    case SurfaceKind_SphereMoving: {
      NewSurface.SphereMoving = ObjCopyFromRef(sphere_moving, SurfaceData);
    } break;
    case SurfaceKind_RectXY: {
      NewSurface.Rect = ObjCopyFromRef(rect, SurfaceData);
    } break;
    case SurfaceKind_RectXZ: {
      NewSurface.Rect = ObjCopyFromRef(rect, SurfaceData);
    } break;
    case SurfaceKind_RectYZ: {
      NewSurface.Rect = ObjCopyFromRef(rect, SurfaceData);
    } break;
    case SurfaceKind_Box: {
      if(WorldSurfacesStaticStackFull(World)) { return NULL; }
      NewSurface.Box = ObjCopyFromRef(box, SurfaceData);
      v3f64 min   = NewSurface.Box.min;
      v3f64 max   = NewSurface.Box.max;
      u32   MatId = NewSurface.Box.MatId;
      rect Sides[6] = {0};
      Sides[0] = SurfaceRectXYInit(min.x, max.x,   min.y, max.y,   max.z,  MatId);
      Sides[1] = SurfaceRectXYInit(min.x, max.x,   min.y, max.y,   min.z,  MatId);
      Sides[2] = SurfaceRectXZInit(min.x, max.x,   min.z, max.z,   max.y,  MatId);
      Sides[3] = SurfaceRectXZInit(min.x, max.x,   min.z, max.z,   min.y,  MatId);
      Sides[4] = SurfaceRectYZInit(min.y, max.y,   min.z, max.z,   max.x,  MatId);
      Sides[5] = SurfaceRectYZInit(min.y, max.y,   min.z, max.z,   min.x,  MatId);
      NewSurface.Box.SidesList = World->SurfaceStaticNext;
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXY, &Sides[0]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXY, &Sides[1]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXZ, &Sides[2]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXZ, &Sides[3]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectYZ, &Sides[4]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectYZ, &Sides[5]);
    }break;
    case SurfaceKind_TransformedInst: {
      NewSurface.TransformedInst = ObjCopyFromRef(transformed_inst, SurfaceData);
    }break;
    case SurfaceKind_ConstantMedium: {
      NewSurface.ConstantMedium = ObjCopyFromRef(constant_medium, SurfaceData);
    }break;
    case SurfaceKind_FlipFace:
    { 
      NewSurface.FlipFace = ObjCopyFromRef(flip_face, SurfaceData);
    } break;
    case SurfaceKind_BVHNode: { Assert(!"Invalid Codepath"); } break;
  }
  WriteToRef(World->SurfaceNext, NewSurface);
  Result = World->SurfaceNext;
  World->SurfaceNext++;
  World->SurfaceCount++;
  return Result;
}
surface *WorldSurfaceStaticAdd(world *World, surface_kind Kind, void *SurfaceData)
{
  if(WorldSurfacesStaticStackFull(World)) { return NULL; }
  surface *Result = NULL;
  surface NewSurface = { .Kind = Kind };
  switch(Kind)
  {
    case SurfaceKind_Sphere: {
      NewSurface.Sphere = ObjCopyFromRef(sphere, SurfaceData);
    } break;
    case SurfaceKind_SphereMoving: {
      NewSurface.SphereMoving = ObjCopyFromRef(sphere_moving, SurfaceData);
    } break;
    case SurfaceKind_RectXY: {
      NewSurface.Rect = ObjCopyFromRef(rect, SurfaceData);
    } break;
    case SurfaceKind_RectXZ: {
      NewSurface.Rect = ObjCopyFromRef(rect, SurfaceData);
    } break;
    case SurfaceKind_RectYZ: {
      NewSurface.Rect = ObjCopyFromRef(rect, SurfaceData);
    } break;
    case SurfaceKind_Box: {
      NewSurface.Box = ObjCopyFromRef(box, SurfaceData);
      v3f64 min   = NewSurface.Box.min;
      v3f64 max   = NewSurface.Box.max;
      u32   MatId = NewSurface.Box.MatId;
      rect Sides[6] = {0};
      Sides[0] = SurfaceRectXYInit(min.x, max.x,   min.y, max.y,   max.z,  MatId);
      Sides[1] = SurfaceRectXYInit(min.x, max.x,   min.y, max.y,   min.z,  MatId);
      Sides[2] = SurfaceRectXZInit(min.x, max.x,   min.z, max.z,   max.y,  MatId);
      Sides[3] = SurfaceRectXZInit(min.x, max.x,   min.z, max.z,   min.y,  MatId);
      Sides[4] = SurfaceRectYZInit(min.y, max.y,   min.z, max.z,   max.x,  MatId);
      Sides[5] = SurfaceRectYZInit(min.y, max.y,   min.z, max.z,   min.x,  MatId);
      NewSurface.Box.SidesList = World->SurfaceStaticNext;
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXY, &Sides[0]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXY, &Sides[1]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXZ, &Sides[2]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectXZ, &Sides[3]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectYZ, &Sides[4]);
      WorldSurfaceStaticAdd(World, SurfaceKind_RectYZ, &Sides[5]);
    }break;
    case SurfaceKind_TransformedInst: {
      NewSurface.TransformedInst = ObjCopyFromRef(transformed_inst, SurfaceData);
    }break;
    case SurfaceKind_ConstantMedium: {
      NewSurface.ConstantMedium = ObjCopyFromRef(constant_medium, SurfaceData);
    }break;
    case SurfaceKind_FlipFace:
    { 
      NewSurface.FlipFace = ObjCopyFromRef(flip_face, SurfaceData);
    } break;
    
    case SurfaceKind_BVHNode: { Assert(!"Invalid Codepath"); } break;
  }
  WriteToRef(World->SurfaceStaticNext, NewSurface);
  Result = World->SurfaceStaticNext;
  World->SurfaceStaticNext++;
  World->SurfaceStaticCount++;
  return Result;
}
u32 WorldTextureAdd(world *World, texture_kind Kind,
                    v3f64 Color,
                    u32 CheckerTexIdA, u32 CheckerTexIdB,
                    u32 NoiseId, f64 NoiseScale, const char *Path)
{
  if(WorldTextureStackFull(World)) { return TEXTURE_INVALID_ID; }
  texture NewTex = { .Kind = Kind };
  switch(Kind)
  {
    case TextureKind_SolidColor: {
      NewTex.Color = Color;
    }break;
    case TextureKind_Checker: {
      NewTex.CheckerTex[0] = WorldTextureGetFromId(World, CheckerTexIdA);
      NewTex.CheckerTex[1] = WorldTextureGetFromId(World, CheckerTexIdB);
    }break;
    case TextureKind_Noise: {
      NewTex.Perlin = WorldNoiseGetFromId(World, NoiseId);
      NewTex.Scale  = NoiseScale;
    }break;
    case TextureKind_Image: {
      NewTex.Data  = ImageLoadFromFile(Path, &NewTex.Width, &NewTex.Height, &NewTex.Stride, 3, &World->Arena);
      NewTex.Pitch = NewTex.Width*NewTex.Stride;
    }break;
  }
  WriteToRef(World->TextureNext, NewTex);
  // NOTE(MIGUEL): This could be an issue with overflow and computiong wrong index
  u32 TexId = GetElementIndex(World->Textures, World->TextureNext, texture);
  Assert(TexId != TEXTURE_INVALID_ID);
  World->TextureNext++;
  World->TextureCount++;
  return TexId;
}
u32 WorldMaterialAdd(world *World, material_kind Kind, u32 TexId, f64 Fuzziness, f64 IndexOfRefraction)
{
  if(WorldMaterialStackFull(World)) { return MATERIAL_INVALID_ID; }
  // NOTE(MIGUEL): these should be assigned via switch so that the expected field values per kind is clear
  material NewMat = { 
    .Kind = Kind,
    .TexId = TexId,
    .Fuzz = Clamp(Fuzziness, 0.0, 1.0),
    .IndexOfRefraction = IndexOfRefraction,
    //.EmmitColor = EmmitColor,
  };
  WriteToRef(World->MaterialNext, NewMat);
  // NOTE(MIGUEL): This could be an issue with overflow and computiong wrong index
  u32 MatId = GetElementIndex(World->Materials, World->MaterialNext, material);
  Assert(MatId != MATERIAL_INVALID_ID);
  World->MaterialNext++;
  World->MaterialCount++;
  return MatId;
}
u32 WorldNoiseAdd(world *World, noise_kind Kind)
{
  if(WorldNoiseStackFull(World)) { return NOISE_INVALID_ID; }
  noise NewNoise = { .Kind = Kind }; //this happens in noise init but is here for consistency
  switch(Kind)
  {
    case NoiseKind_Perin: {
      NewNoise = NoiseInit(Kind);
    }break;
  }
  WriteToRef(World->NoiseNext, NewNoise);
  // NOTE(MIGUEL): This could be an issue with overflow and computiong wrong index
  u32 NoiseId = (u32)((u64)World->NoiseNext-(u64)World->Noises)/sizeof(noise);
  Assert(NoiseId != NOISE_INVALID_ID);
  World->NoiseNext++;
  World->NoiseCount++;
  return NoiseId;
}
u32 WorldLightAdd(world *World, light_kind Kind, surface *Surface)
{
  if(WorldLightStackFull(World)) { return LIGHT_INVALID_ID; }
  light NewLight = { Kind, Surface };
  
  u32 LightId = GetElementIndex(World->Lights, World->LightNext, light);
  WriteToRef(World->LightNext, NewLight);
  World->LightNext++;
  World->LightCount++;
  return LightId;
}

//~ ACCESSORS
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
material *WorldMaterialGetFromId(world *World, u32 MatId)
{
  MatId = (MatId == MATERIAL_INVALID_ID)?World->DefaultMatId:MatId;
  material *Mat = World->Materials + MatId;
  u64 Start   = (u64)World->Materials;
  u64 End     = (u64)World->MaterialNext;
  u64 Address = (u64)Mat;
  if(!(Start<Address && Address<End)) { return &World->Materials[World->DefaultMatId]; }
  return Mat;
}
noise *WorldNoiseGetFromId(world *World, u32 NoiseId)
{
  NoiseId = (NoiseId == NOISE_INVALID_ID)?World->DefaultNoiseId:NoiseId;
  noise *Noise = World->Noises + NoiseId;
  u64 Start   = (u64)World->Noises;
  u64 End     = (u64)World->NoiseNext;
  u64 Address = (u64)Noise;
  if(!(Start<Address && Address<End)) { return &World->Noises[World->DefaultNoiseId]; }
  return Noise;
}

//~ WORLD TRAVERSAL
surface *WorldHit(world *World, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt, b32 UseBVH)
{
  surface *Result = (UseBVH?
                     SurfaceBVHListHit(World->BVHRoots, World->BVHRootCount, HitInfo, Ray, Mint, Maxt):
                     SurfaceListHit   (World->Surfaces, World->SurfaceCount, HitInfo, Ray, Mint, Maxt));
  return Result;
}