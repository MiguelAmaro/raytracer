#ifndef RTSCENES_H
#define RTSCENES_H


void SceneBVHTest(world *World, camera *Camera, f64 AspectRatio)
{
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 10.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.1;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  f64 Fuzz = 0.0;
  v3f64 AlbedoR = V3f64(1.0,0.0,0.0);
  v3f64 AlbedoG = V3f64(0.0,1.0,0.0);
  v3f64 AlbedoB = V3f64(1.0,0.0,1.0);
  
  v3f64 CenterA = V3f64(0.0, 0.2, 0.0);
  v3f64 CenterB = V3f64(4.0, 0.2, 4.0);
  v3f64 CenterC = V3f64(2.0, 0.2, 2.0);
  u32 TA = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoR, 0,0,0,0, NULL);
  u32 TB = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoG, 0,0,0,0, NULL);
  u32 TC = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoB, 0,0,0,0, NULL);
  u32 MA = WorldMaterialAdd(World, MaterialKind_Metal, TA, Fuzz,0.0);
  u32 MB = WorldMaterialAdd(World, MaterialKind_Metal, TB, Fuzz,0.0);
  u32 MC = WorldMaterialAdd(World, MaterialKind_Metal, TC, Fuzz,0.0);
  sphere Sphere[3] = {0};
  Sphere[0] = SurfaceSphereInit(CenterA, 0.2, MA);
  Sphere[1] = SurfaceSphereInit(CenterB, 0.2, MB);
  Sphere[2] = SurfaceSphereInit(CenterC, 0.2, MC);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[0]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[1]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[2]);
  BVHInit(&World->BVHRoot, World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneTwoSpheres(world *World, camera *Camera, f64 AspectRatio)
{
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   Aperture = 0.0;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, 20.0, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  v3f64 AlbedoR = V3f64(0.2,0.3,0.1);
  v3f64 AlbedoG = V3f64(0.9,0.9,0.9);
  
  v3f64 CenterA = V3f64(0.0,  10.0, 0.0);
  v3f64 CenterB = V3f64(0.0, -10.0, 0.0);
  u32 TA = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoR, 0,0,0,0, NULL);
  u32 TB = WorldTextureAdd(World, TextureKind_SolidColor, AlbedoG, 0,0,0,0, NULL);
  u32 TChecker = WorldTextureAdd(World, TextureKind_Checker, V3f64(0.0,0.0,0.0), TA,TB,0,0, NULL);
  
  u32 MA = WorldMaterialAdd(World, MaterialKind_Lambert, TChecker,0.0,0.0);
  sphere Sphere[2] = {0};
  Sphere[0] = SurfaceSphereInit(CenterA,  10.0, MA);
  Sphere[1] = SurfaceSphereInit(CenterB, -10.0, MA);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[0]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[1]);
  BVHInit(&World->BVHRoot, World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneTwoPerlinSpheres(world *World, camera *Camera, f64 AspectRatio)
{
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.0;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  v3f64 AlbedoR = V3f64(0.2,0.3,0.1);
  
  v3f64 CenterA = V3f64(0.0, -1000.0, 0.0);
  v3f64 CenterB = V3f64(0.0,  2.0, 0.0);
  u32 NA = WorldNoiseAdd(World, NoiseKind_Perin);
  u32 TA = WorldTextureAdd(World, TextureKind_Noise, AlbedoR, 0,0, NA,4, NULL);
  
  u32 MA = WorldMaterialAdd(World, MaterialKind_Lambert, TA,0.0,0.0);
  sphere Sphere[2] = {0};
  Sphere[0] = SurfaceSphereInit(CenterA, 1000.0, MA);
  Sphere[1] = SurfaceSphereInit(CenterB, 2.0, MA);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[0]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[1]);
  BVHInit(&World->BVHRoot, World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneEarthSolo(world *World, camera *Camera, f64 AspectRatio)
{
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.0;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  u32 TA = WorldTextureAdd(World, TextureKind_Image, V3f64(0.0,0.0,0.0), 0,0,0,0, "F:\\Dev\\raytracer\\assets\\earthmap.jpg");
  u32 MA = WorldMaterialAdd(World, MaterialKind_Lambert, TA,0.0,0.0);
  
  sphere Sphere = SurfaceSphereInit(V3f64(0,0,0), 2.0, MA);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere);
  
  BVHInit(&World->BVHRoot, World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneRandom(world *World, camera *Camera, f64 AspectRatio)
{
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 10.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.1;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  u32 TGroundA = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.5,0.5,0.5), 0,0,0,0,NULL);
  u32 TGroundB = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.8,0.5,0.0), 0,0,0,0,NULL);
  u32 TGroundChecker = WorldTextureAdd(World, TextureKind_Checker, V3f64(0.0,0.5,0.8), TGroundA, TGroundB,0,0,NULL);
  u32 MGround = WorldMaterialAdd(World, MaterialKind_Lambert, TGroundChecker, 0.0, 0.0);
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
        u32 SphereTexId = TEXTURE_INVALID_ID;
        u32 SphereMatId = MATERIAL_INVALID_ID;
        if(RandMat<0.4)
        {
          // diffuse
          v3f64 Albedo = Mul(V3f64RandUni(), V3f64RandUni());
          SphereTexId  = WorldTextureAdd(World, TextureKind_SolidColor, Albedo,0,0,0,0,NULL);
          SphereMatId  = WorldMaterialAdd(World, MaterialKind_Lambert, SphereTexId, 0.0,0.0);
          v3f64 Center2 = Add(Center, V3f64(0.0,RandF64Uni()*0.5,0.0));
          sphere_moving SphereMoving = SurfaceSphereMovingInit(Center, Center2, 0.0, 1.0, 0.2, SphereMatId);
          WorldSurfaceAdd(World, SurfaceKind_SphereMoving, &SphereMoving);
        }
        else if(RandMat<0.65)
        {
          // metal
          v3f64 Albedo = Add(Scale(V3f64RandUni(), 0.5), V3f64(0.5,0.5,0.5));
          f64   Fuzz   = 0.5 + 0.5*RandF64Uni();
          SphereTexId  = WorldTextureAdd(World, TextureKind_SolidColor, Albedo,0,0,0,0,NULL);
          SphereMatId  = WorldMaterialAdd(World, MaterialKind_Metal, SphereTexId, Fuzz,0.0);
          sphere Sphere = SurfaceSphereInit(Center, 0.2, SphereMatId);
          WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere);
        }
        else
        {
          // glass
          v3f64 DummyAlbedo = V3f64(1.0,1.0,1.0);
          SphereTexId  = WorldTextureAdd(World, TextureKind_SolidColor, DummyAlbedo,0,0,0,0,NULL);
          SphereMatId  = WorldMaterialAdd(World, MaterialKind_Dielectric, SphereTexId, 0.0,1.5);
          sphere Sphere = SurfaceSphereInit(Center, 0.2, SphereMatId);
          WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere);
        }
      }
    }
  }
  u32 TA = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(1.0,1.0,1.0),0,0,0,0,NULL);
  u32 TB = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.4,0.2,0.1),0,0,0,0,NULL);
  u32 TC = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.7,0.6,0.5),0,0,0,0,NULL);
  u32 M1 = WorldMaterialAdd(World, MaterialKind_Dielectric, TA, 0.3, 1.5);
  u32 M2 = WorldMaterialAdd(World, MaterialKind_Lambert, TB, 0.3, 1.5);
  u32 M3 = WorldMaterialAdd(World, MaterialKind_Metal, TC, 0.0, 0.0);
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
