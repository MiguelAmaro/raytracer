#ifndef RTSCENES_H
#define RTSCENES_H


void SceneBVHTest(world *World)
{
  f64 Fuzz = 0.0;
  v3f64 AlbedoR = V3f64(1.0,0.0,0.0);
  v3f64 AlbedoG = V3f64(0.0,1.0,0.0);
  v3f64 AlbedoB = V3f64(1.0,0.0,1.0);
  
  v3f64 CenterA = V3f64(0.0, 0.2, 0.0);
  v3f64 CenterB = V3f64(4.0, 0.2, 4.0);
  v3f64 CenterC = V3f64(2.0, 0.2, 2.0);
  u32 TA = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoR);
  u32 TB = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoG);
  u32 TC = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoB);
  u32 MA = WorldMaterialAdd(World, MaterialKind_Metal, TA, Fuzz,0.0);
  u32 MB = WorldMaterialAdd(World, MaterialKind_Metal, TB, Fuzz,0.0);
  u32 MC = WorldMaterialAdd(World, MaterialKind_Metal, TC, Fuzz,0.0);
  sphere Sphere[3] = {0};
  Sphere[0] = SurfaceSphereInit(CenterA, 1.0, MA);
  Sphere[1] = SurfaceSphereInit(CenterB, 1.0, MB);
  Sphere[2] = SurfaceSphereInit(CenterC, 1.0, MC);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[0]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[1]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[2]);
  BVHInit(&World->BVHRoot, World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneRandom(world *World)
{
  u32 TGround = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.5,0.5,0.5));
  u32 MGround = WorldMaterialAdd(World, MaterialKind_Lambert, TGround, 0.0, 0.0);
  sphere WorldSphere = SurfaceSphereInit(V3f64(0.0,-1000.5,-1.0), 1000.0, MGround);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &WorldSphere);
  for(int a=-11;a<11; a++)
  {
    for(int b=-11;b<11;b++)
    {
      f64 RandMat = RandF64Uni();
      v3f64 Center = V3f64(a + 0.9*RandF64Uni(), 0.2, b + 0.9*RandF64Uni());
      if(Length(Sub(Center, V3f64(4.0,0.2,0.0)))>0.9)
      {
        u32 SphereMatId = MATERIAL_INVALID_ID;
        if(RandMat<0.8)
        {
          // diffuse
          v3f64 Albedo = Mul(V3f64RandUni(), V3f64RandUni());
          SphereMatId  = WorldMaterialAdd(World, MaterialKind_Lambert, Albedo, 0.0,0.0);
          v3f64 Center2 = Add(Center, V3f64(0.0,RandF64Uni()*0.5,0.0));
          sphere_moving SphereMoving = SurfaceSphereMovingInit(Center, Center2, 0.0, 1.0, 0.2, SphereMatId);
          WorldSurfaceAdd(World, SurfaceKind_SphereMoving, &SphereMoving);
        }
        else if(RandMat<0.95)
        {
          // metal
          v3f64 Albedo = Add(Scale(V3f64RandUni(), 0.5), V3f64(0.5,0.5,0.5));
          f64   Fuzz   = 0.5 + 0.5*RandF64Uni();
          SphereMatId  = WorldMaterialAdd(World, MaterialKind_Metal, Albedo, Fuzz,0.0);
          sphere Sphere = SurfaceSphereInit(Center, 0.2, SphereMatId);
          WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere);
        }
        else
        {
          // glass
          v3f64 DummyAlbedo = V3f64(1.0,1.0,1.0);
          SphereMatId       = WorldMaterialAdd(World, MaterialKind_Dielectric, DummyAlbedo, 0.0,1.5);
          sphere Sphere = SurfaceSphereInit(Center, 0.2, SphereMatId);
          WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere);
        }
      }
    }
  }
  u32 M1 = WorldMaterialAdd(World, MaterialKind_Dielectric, V3f64(1.0,1.0,1.0), 0.3, 1.5);
  u32 M2 = WorldMaterialAdd(World, MaterialKind_Lambert, V3f64(0.4,0.2,0.1), 0.3, 1.5);
  u32 M3 = WorldMaterialAdd(World, MaterialKind_Metal, V3f64(0.7,0.6,0.5), 0.0, 0.0);
  sphere Sphere[3] = {0};
  Sphere[0] = SurfaceSphereInit(V3f64( 0.0,1.0,0.0), 1.0, M3);
  Sphere[1] = SurfaceSphereInit(V3f64(-4.0,1.0,0.0), 1.0, M2);
  Sphere[2] = SurfaceSphereInit(V3f64( 4.0,1.0,0.0), 1.0, M1);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[0]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[1]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[2]);
  
  BVHInit(&World->BVHRoot, World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}

#endif //RTSCENES_H
