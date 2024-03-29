#ifndef RTSCENES_H
#define RTSCENES_H

//BUILDER
void MakePointLight(world *World, v3f64 Pos, f64 Radius, v3f64 Color)
{
  u32      TId  = WorldTextureAdd(World, TextureKind_SolidColor, Color, 0,0,0,0,NULL);
  u32      MId  = WorldMaterialAdd(World, MaterialKind_DiffuseLight, TId, 0.0, 0.0);
  sphere   Data = SurfaceSphereInit(Pos, Radius, MId);
  surface *Ref  = WorldSurfaceAdd(World, SurfaceKind_Sphere, &Data);
  WorldLightAdd(World, LightKind_Point, Ref);
  return;
}
void MakeColoredSphere(world *World, v3f64 Pos, f64 Radius, v3f64 Color, f64 Fuzz)
{
  u32     TId = WorldTextureAdd(World, TextureKind_SolidColor, Color, 0,0,0,0, NULL);
  u32     MId = WorldMaterialAdd(World, MaterialKind_Lambert, TId, Fuzz, 0.0);
  sphere Data = SurfaceSphereInit(Pos, Radius, MId);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Data);
  return;
}


//Scene
void SceneBVHTest(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(1.0,0.827,0.657); //bright orange
  World->DefaultBackground[1] = V3f64(0.39,0.613,0.792); //sky blue
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 10.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.1;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  MakeColoredSphere(World, V3f64(0.0, 0.2, 0.0), 0.2, V3f64(1.0,0.0,0.0), 0.0);
  MakeColoredSphere(World, V3f64(4.0, 0.2, 4.0), 0.2, V3f64(0.0,1.0,0.0), 0.0);
  MakeColoredSphere(World, V3f64(2.0, 0.2, 2.0), 0.2, V3f64(1.0,0.0,1.0), 0.0);
  MakePointLight(World, V3f64(4.0, 0.2, 4.0), 0.02, V3f64(1.0,0.0,1.0));
  
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneTwoSpheres(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(0.7,0.8,1.0);
  World->DefaultBackground[1] = V3f64(0.7,0.8,1.0);
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   Aperture = 0.1;
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
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneTwoPerlinSpheres(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(0.7,0.8,1.0);
  World->DefaultBackground[1] = V3f64(0.7,0.8,1.0);
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.1;
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
  //BVHInit(&World->BVHRoot, World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneEarthSolo(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(0.7,0.8,1.0);
  World->DefaultBackground[1] = V3f64(0.7,0.8,1.0);
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.1;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  u32 TA = WorldTextureAdd(World, TextureKind_Image, V3f64(0.0,0.0,0.0), 0,0,0,0, "F:\\Dev\\raytracer\\assets\\earthmap.jpg");
  u32 MA = WorldMaterialAdd(World, MaterialKind_Lambert, TA,0.0,0.0);
  
  sphere Sphere = SurfaceSphereInit(V3f64(0,0,0), 2.0, MA);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere);
  
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneSimpleLight(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(0.0,0.0,0.0);
  World->DefaultBackground[1] = V3f64(0.0,0.0,0.0);
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(26.0,3.0,6.0);
  v3f64 LookAt   = V3f64(0.0,2.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.1;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  u32 NA = WorldNoiseAdd(World, NoiseKind_Perin);
  u32 TA = WorldTextureAdd(World, TextureKind_Noise, V3f64(0,0,0), 0,0,NA,4,NULL);
  u32 MA = WorldMaterialAdd(World, MaterialKind_Lambert, TA,0.0,0.0);
  
  sphere SphereBig   = SurfaceSphereInit(V3f64(0,-1000,0), 1000.0, MA);
  sphere SphereSmall = SurfaceSphereInit(V3f64(0,2,0), 2.0, MA);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &SphereBig);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &SphereSmall);
  
  u32 TB = WorldTextureAdd(World, TextureKind_Image, V3f64(0,0,0), 0,0,0,0.0,"F:\\Dev\\raytracer\\assets\\earthmap.jpg");
  u32  TL = WorldTextureAdd(World, TextureKind_Checker, V3f64(4,4,4), TA,TB,0,0.0,NULL);
  u32  ML = WorldMaterialAdd(World, MaterialKind_DiffuseLight, TL,0.0,0.0);
  rect RA = SurfaceRectXYInit(3.0,5.0,1.0,3.0,-2.0, ML);
  surface *LightRef = WorldSurfaceAdd(World, SurfaceKind_RectXY, &RA);
  World->Lights[0] = (light){LightKind_Point, LightRef};
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
void SceneRandom(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(1.0,0.827,0.657); //bright orange
  World->DefaultBackground[1] = V3f64(0.39,0.613,0.792); //sky blue
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 8.0; //20.0
  f64   FOV = 20.0; //20.0
  f64   Aperture = 0.04;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  u32 TGroundA = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.45,0.25,0.35), 0,0,0,0,NULL);
  u32 TGroundB = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.48,0.58,0.58), 0,0,0,0,NULL);
  u32 TGroundChecker = WorldTextureAdd(World, TextureKind_Checker, V3f64(0.0,0.5,0.8), TGroundA, TGroundB,0,0,NULL);
  u32 MGround = WorldMaterialAdd(World, MaterialKind_Metal, TGroundChecker, 0.3, 0.0);
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
  u32 TA = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.0,1.0,1.0),0,0,0,0,NULL);
  u32 TB = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(5.4,2.2,2.1),0,0,0,0,NULL);
  u32 TC = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.7,0.6,0.5),0,0,0,0,NULL);
  u32 TG = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.55,0.43,0.2),0,0,0,0,NULL);
  u32 M1 = WorldMaterialAdd(World, MaterialKind_Dielectric, TA, 0.0, 1.2);
  u32 M1A = WorldMaterialAdd(World, MaterialKind_Dielectric, TA, 0.0, -1.2);
  u32 M2 = WorldMaterialAdd(World, MaterialKind_DiffuseLight, TB, 0.0, 0.0);
  u32 M3 = WorldMaterialAdd(World, MaterialKind_Metal, TC, 0.0, 0.0);
  u32 M4 = WorldMaterialAdd(World, MaterialKind_Metal, TG, 0.12, 0.0);
  sphere Sphere[5] = {0};
  Sphere[0] = SurfaceSphereInit(V3f64( 0.0,1.0,0.0), 1.0, M3);
  Sphere[1] = SurfaceSphereInit(V3f64( 2.0,2.0,0.0), 0.5, M2);
  Sphere[2] = SurfaceSphereInit(V3f64( 4.0,1.0,0.0), 1.0, M1);
  Sphere[3] = SurfaceSphereInit(V3f64( 4.0,1.0,0.0), 0.50, M4);
  Sphere[4] = SurfaceSphereInit(V3f64( 4.0,1.0,0.0), 0.70, M1A);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[0]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[1]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[2]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[3]);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &Sphere[4]);
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  for(u32 i=0;i<World->SurfaceCount; i++)
  {
    if(World->Surfaces[i].Kind == SurfaceKind_Sphere &&
       World->Surfaces[i].Sphere.MatId == M2)
    {
      World->Lights[0] = (light){ LightKind_Point, &World->Surfaces[i]};
    }
  }
  return;
}
void SceneCornellBox(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(0.0,0.0,0.0);
  World->DefaultBackground[1] = V3f64(0.0,0.0,0.0);
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(278.0,278.0,-800.0);
  v3f64 LookAt   = V3f64(278.0,278.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 150.0;
  f64   FOV = 40.0;
  f64   Aperture = 0.08;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  
  u32 TR = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.65,0.05,0.05), 0,0,0,0,NULL);
  u32 TW = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.73,0.73,0.73), 0,0,0,0,NULL);
  u32 TG = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.12,0.45,0.15), 0,0,0,0,NULL);
  u32 TL = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(4,4,4), 0,0,0,0,NULL);
  u32 MR = WorldMaterialAdd(World, MaterialKind_Lambert, TR,0.0,0.0);
  u32 MW = WorldMaterialAdd(World, MaterialKind_Lambert, TW,0.0,0.0);
  u32 MG = WorldMaterialAdd(World, MaterialKind_Lambert, TG,0.0,0.0);
  u32 ML = WorldMaterialAdd(World, MaterialKind_DiffuseLight, TL,0.0,0.0);
  rect RYZL = SurfaceRectYZInit(0.0,555.0,
                                0.0,555.0, 555.0, MG);
  rect RYZR = SurfaceRectYZInit(0.0,555.0,
                                0.0,555.0, 0.0, MR);
  WorldSurfaceAdd(World, SurfaceKind_RectYZ, &RYZL);
  WorldSurfaceAdd(World, SurfaceKind_RectYZ, &RYZR);
  
  rect RXZLight = SurfaceRectXZInit(213.0,343.0,   227.0,332.0,   554.0,    ML);
  rect RXZT     = SurfaceRectXZInit(0.0,555.0,     0.0,555.0,     555.0,    MW);
  rect RXZB     = SurfaceRectXZInit(0.0,555.0,     0.0,555.0,     0.0  ,    MW);
  surface *LightRef = WorldSurfaceAdd(World, SurfaceKind_RectXZ, &RXZLight);
  WorldSurfaceAdd(World, SurfaceKind_RectXZ, &RXZT);
  WorldSurfaceAdd(World, SurfaceKind_RectXZ, &RXZB);
  
  rect RXYF = SurfaceRectXYInit(0.0,555.0,  0.0,555.0,  555.0, MW);
  WorldSurfaceAdd(World, SurfaceKind_RectXY, &RXYF);
  
  box BB = SurfaceBoxInit(V3f64(0.0, 0.0, 0.0), V3f64(165.0, 335.0, 165.0), MW);
  box BS = SurfaceBoxInit(V3f64(0.0, 0.0, 0.0), V3f64(165.0, 165.0, 165.0), MW);
  surface *SBoxB = WorldSurfaceStaticAdd(World, SurfaceKind_Box, &BB);
  surface *SBoxS = WorldSurfaceStaticAdd(World, SurfaceKind_Box, &BS);
  transformed_inst XFB = SurfaceTransformedInstanceInit(TransformKind_RotateY | 
                                                        TransformKind_Translate, SBoxB, V3f64(265.0,0.0,295.0), DegToRad64(15.0));
  transformed_inst XFS = SurfaceTransformedInstanceInit(TransformKind_RotateY |
                                                        TransformKind_Translate, SBoxS, V3f64(130.0,0.0,65.0), DegToRad64(-18.0));
  WorldSurfaceAdd(World, SurfaceKind_TransformedInst, &XFB);
  WorldSurfaceAdd(World, SurfaceKind_TransformedInst, &XFS);
  
  World->Lights[0] = (light){ LightKind_Point, LightRef};
  Assert(&World->Lights[0]);
  
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  
  return;
}
void SceneTestCornellBox(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(0.8,0.83,0.9);
  World->DefaultBackground[1] = V3f64(0.8,0.83,0.9);
  // TODO(MIGUEL): fix this
  // CAMERA
  v3f64 LookFrom = V3f64(0.0,2000.0,-100.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 150.0;
  f64   FOV = 40.0;
  f64   Aperture = 0.08;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  
  u32 TR = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.65,0.05,0.05), 0,0,0,0,NULL);
  u32 TW = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.73,0.73,0.73), 0,0,0,0,NULL);
  u32 TG = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.12,0.45,0.15), 0,0,0,0,NULL);
  u32 MR = WorldMaterialAdd(World, MaterialKind_Lambert, TR,0.0,0.0);
  u32 MW = WorldMaterialAdd(World, MaterialKind_Lambert, TW,0.0,0.0);
  u32 MG = WorldMaterialAdd(World, MaterialKind_Lambert, TG,0.0,0.0);
  rect RYZL = SurfaceRectYZInit(   0.0, 600.0,
                                -300.0, 300.0, -300.0, MG);
  rect RYZR = SurfaceRectYZInit(   0.0, 600.0,
                                -300.0, 300.0,  300.0, MR);
  WorldSurfaceAdd(World, SurfaceKind_RectYZ, &RYZL);
  WorldSurfaceAdd(World, SurfaceKind_RectYZ, &RYZR);
  
  rect RXZB     = SurfaceRectXZInit(-300.0, 300.0,
                                    -300.0, 300.0, 0.0, MW);
  WorldSurfaceAdd(World, SurfaceKind_RectXZ, &RXZB);
  
  rect RXYF = SurfaceRectXYInit(-300.0,300.0,
                                0.0   ,600.0, 300.0, MW);
  WorldSurfaceAdd(World, SurfaceKind_RectXY, &RXYF);
  
  f64 CenterXY = 300.0;
  f64 BToSOffsetZ = 0.0;
  f64 SizeB = 50.0;
  f64 SizeS = 200.0;
  box BB = SurfaceBoxInit(V3f64(-SizeB, 0.0, -SizeB), V3f64(SizeB, 600.0, SizeB), MW);
  box BS = SurfaceBoxInit(V3f64(-SizeS, 0.0, -SizeS), V3f64(SizeS, 200.0, SizeS), MR);
  box BM = SurfaceBoxInit(V3f64(-SizeS*0.7, 0.0, -SizeS*0.7), V3f64(SizeS*0.7, 400.0, SizeS*0.7), MG);
  surface *SBoxB = WorldSurfaceStaticAdd(World, SurfaceKind_Box, &BB);
  surface *SBoxS = WorldSurfaceStaticAdd(World, SurfaceKind_Box, &BS);
  surface *SBoxM = WorldSurfaceStaticAdd(World, SurfaceKind_Box, &BM);
  transformed_inst XFB = SurfaceTransformedInstanceInit(TransformKind_RotateY, SBoxB, V3f64(CenterXY,0.0,0.0), Pi64/4.0);
  transformed_inst XFS = SurfaceTransformedInstanceInit(TransformKind_Translate, SBoxS, V3f64(CenterXY,0.0,BToSOffsetZ), Pi64/4.0);
  transformed_inst XFM = SurfaceTransformedInstanceInit(TransformKind_RotateY |
                                                        TransformKind_Translate, SBoxM, V3f64(CenterXY,0.0,BToSOffsetZ), Pi64/4.0);
  WorldSurfaceAdd(World, SurfaceKind_TransformedInst, &XFB);
  WorldSurfaceAdd(World, SurfaceKind_TransformedInst, &XFS);
  WorldSurfaceAdd(World, SurfaceKind_TransformedInst, &XFM);
  
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}
//void SceneCornellSmoke(world *World, camera *Camera, f64 AspectRatio){ return; }
void SceneFinal(world *World, camera *Camera, f64 AspectRatio)
{ 
  //CAMERA
  v3f64 LookFrom = V3f64(0.0,2000.0,-100.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 150.0;
  f64   FOV = 40.0;
  f64   Aperture = 0.08;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  WorldBVHRootListInit(World, 10);
  
  u32 BVHCount = 0;
  u32 LastSurfaceCount = 0;
  int BoxPerSide = 20;
  u32 TGround = WorldTextureAdd (World, TextureKind_SolidColor, V3f64(0.48, 0.83, 0.53), 0,0,0,0.0,NULL);
  u32 MGround = WorldMaterialAdd(World, MaterialKind_Lambert, TGround, 0.0,0.0);
  MakePointLight(World, V3f64(.0,3.0,0.0), 3.0, V3f64(1.,1.,1.0));
  for(int i=0;i<BoxPerSide;i++)
  {
    for(int j=0;j<BoxPerSide;j++)
    {
      f64 w = -100.0;
      f64 x0 = 1000.0 + i*w;
      f64 z0 = 1000.0 + j*w;
      f64 y0 = 0.0;
      f64 x1 = x0 + w;
      f64 y1 = RandF64Range(1.0, 101.0);
      f64 z1 = z0 + w;
      box Box = SurfaceBoxInit(V3f64(x0,y0,z0), V3f64(x1,y1,z1), MGround);
      WorldSurfaceAdd(World, SurfaceKind_Box, &Box);
    }
  }
  BVHInit(&World->BVHRoots[BVHCount++], World->Surfaces, LastSurfaceCount, World->SurfaceCount-1, 0.0,1.0, &World->Arena);
  LastSurfaceCount = World->SurfaceCount;
  
  u32 TLight = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(7,7,7), 0,0,0,0,NULL);
  WorldMaterialAdd(World, MaterialKind_DiffuseLight, TLight, 0.0,0.0);
  
  
  // MOVING SPHERE
  v3f64 Center1 = V3f64(400,400,400);
  v3f64 Center2 = Add(Center1,V3f64(30,0,0));
  u32   TSphereMoving = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.7,0.3,0.1), 0,0,0,0.0,NULL);
  u32   MSphereMoving = WorldMaterialAdd(World, MaterialKind_Lambert, TSphereMoving, 0.0,0.0);
  sphere_moving SphereMoving = SurfaceSphereMovingInit(Center1, Center2, 0.0, 1.0, 50.0, MSphereMoving);
  WorldSurfaceAdd(World, SurfaceKind_SphereMoving, &SphereMoving);
  
  
  // GLASS & METAL
  sphere SGlass = SurfaceSphereInit(V3f64(260,150,45), 50.0, WorldMaterialAdd(World, MaterialKind_Dielectric,0,0.0,1.5));
  sphere SMetal = SurfaceSphereInit(V3f64(0,150,145), 50.0, WorldMaterialAdd(World, MaterialKind_Metal,0,1.0,0.0));
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &SGlass);
  WorldSurfaceAdd(World, SurfaceKind_Sphere, &SMetal);
  
  // CONSTANT MEDIUM
  //first
  sphere SBoundary = SurfaceSphereInit(V3f64(360,150,145), 70.0, WorldMaterialAdd(World, MaterialKind_Dielectric,0,0.0,1.5));
  surface *BoundarySurface = WorldSurfaceStaticAdd(World, SurfaceKind_Sphere, &SBoundary);
  u32 TBoundary = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(0.2,0.4,0.9),0,0,0,0.0,NULL);
  u32 MBoundary = WorldMaterialAdd(World,MaterialKind_Lambert,TBoundary,0.0,0.0);
  constant_medium Medium  = SurfaceConstantMediumInit(BoundarySurface, 0.2, MBoundary);
  WorldSurfaceAdd(World, SurfaceKind_ConstantMedium, &Medium);
  //second
  sphere SBoundary2 = SurfaceSphereInit(V3f64(0,0,0), 5000.0, WorldMaterialAdd(World, MaterialKind_Dielectric,0,0.0,1.5));
  surface *BoundarySurface2 = WorldSurfaceStaticAdd(World, SurfaceKind_Sphere, &SBoundary2);
  u32 TBoundary2 = WorldTextureAdd(World, TextureKind_SolidColor, V3f64(1.0,1.0,1.0),0,0,0,0.0,NULL);
  u32 MBoundary2 = WorldMaterialAdd(World,MaterialKind_Lambert,TBoundary2,0.0,0.0);
  constant_medium Medium2  = SurfaceConstantMediumInit(BoundarySurface2, 0.2, MBoundary2);
  WorldSurfaceAdd(World, SurfaceKind_ConstantMedium, &Medium2);
  
  BVHInit(&World->BVHRoots[BVHCount++], World->Surfaces, LastSurfaceCount, World->SurfaceCount-1, 0.0,1.0, &World->Arena);
  LastSurfaceCount = World->SurfaceCount;
  
  //CUBE OF SPHERES
  int ns = 1000;
  for(int j=0;j<ns;j++)
  {
    
  }
  BVHInit(&World->BVHRoots[BVHCount], World->Surfaces, 0, World->SurfaceCount-1, 0.0,1.0, &World->Arena);
  
  
  return; 
} 
void SceneTestNanIssue(world *World, camera *Camera, f64 AspectRatio)
{
  World->DefaultBackground[0] = V3f64(0.7,0.8,1.0);
  World->DefaultBackground[1] = V3f64(0.7,0.8,1.0);
  // CAMERA
  v3f64 LookFrom = V3f64(13.0,2.0,3.0);
  v3f64 LookAt   = V3f64(0.0,0.0,0.0);
  v3f64 RelUp    = V3f64(0.0,1.0,0.0);
  f64   DistToFocus = 20.0;
  f64   FOV = 20.0;
  f64   Aperture = 0.1;
  WriteToRef(Camera, CameraInit(LookFrom, LookAt, RelUp, FOV, AspectRatio, Aperture, DistToFocus, 0.0, 1.0));
  
  MakeColoredSphere(World, V3f64(0.0,-1000.5,-1.0), 1000.0, V3f64(0.5,0.5,0.5), 0.0);
  MakeColoredSphere(World, V3f64(0.0,0.0,0.0), 1000.0, V3f64(0.1,0.9,0.1), 0.0);
  MakePointLight   (World, V3f64(0.0,2.0,0.0), 1000.0, V3f64(5.0,7.0,8.0));
  
  //Make BVH
  WorldBVHRootListInit(World, 1);
  BVHInit(&World->BVHRoots[0], World->Surfaces, 0, World->SurfaceCount, 0.0, 1.0, &World->Arena);
  return;
}

#endif //RTSCENES_H
