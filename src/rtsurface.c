
//~ SURFACE INITIALIZERS

inline plane SurfacePlaneInit(v3f64 Normal, f64 Dist,  u32 MatId)
{
  plane Result = { .Normal = Normal, .Dist = Dist, .MatId = MatId };
  return Result;
}
inline sphere SurfaceSphereInit(v3f64 Pos, f64 Radius,  u32 MatId)
{
  sphere Result = { .Pos = Pos, .Radius = Radius, .MatId = MatId };
  return Result;
}
inline sphere_moving SurfaceSphereMovingInit(v3f64 Pos0, v3f64 Pos1,
                                             f64 Time0, f64 Time1,
                                             f64 Radius,  u32 MatId)
{
  sphere_moving Result = {
    .Pos0   = Pos0  , .Pos1   = Pos1,
    .Time0  = Time0 , .Time1  = Time1,
    .Radius = Radius, .MatId  = MatId
  };
  return Result;
}
inline rect SurfaceRectXYInit(f64 x0, f64 x1, f64 y0, f64 y1, f64 Offset, u32 MatId)
{
  rect Result = {
    .Points = R3f64(x0,y0,0.0,x1,y1,0.0),
    .Offset = Offset,
    .MatId = MatId,
  };
  return Result;
}
inline rect SurfaceRectXZInit(f64 x0, f64 x1, f64 z0, f64 z1, f64 Offset, u32 MatId)
{
  rect Result = {
    .Points = R3f64(x0,0.0,z0,x1,0.0,z1),
    .Offset = Offset,
    .MatId = MatId,
  };
  return Result;
}
inline rect SurfaceRectYZInit(f64 y0, f64 y1, f64 z0, f64 z1, f64 Offset, u32 MatId)
{
  rect Result = {
    .Points = R3f64(0.0,y0,z0,0.0,y1,z1),
    .Offset = Offset,
    .MatId = MatId,
  };
  return Result;
}
inline box SurfaceBoxInit(v3f64 min, v3f64 max, u32 MatId)
{
  box Result = { .min = min, .max = max, .MatId = MatId };
  return Result;
}
inline transformed_inst SurfaceTransformedInstanceInit(transform_kind Kind, surface *Instance, v3f64 Offset, f64 Radians)
{
  transformed_inst Result = { .Kind = Kind, .Instance = Instance };
  if(Kind & TransformKind_RotateY)
  {
    Result.CosTheta = Cos(Radians);
    Result.SinTheta = Sin(Radians);
    Result.HasBox = AABBInitSurface(Instance, 0.0, 1.0, &Result.AABB);
    v3f64 min = V3f64( Infintyf64(),  Infintyf64(),  Infintyf64());
    v3f64 max = V3f64(-Infintyf64(), -Infintyf64(), -Infintyf64());
    for(s32 i=0;i<2;i++)
    {
      for(s32 j=0;j<2;j++)
      {
        for(s32 k=0;k<2;k++)
        {
          f64 x = i*Result.AABB.max.x + (1-i)*Result.AABB.min.x;
          f64 y = j*Result.AABB.max.y + (1-j)*Result.AABB.min.y;
          f64 z = k*Result.AABB.max.z + (1-k)*Result.AABB.min.z;
          f64 NewX =  Result.CosTheta*x + Result.SinTheta*z;
          f64 NewZ = -Result.SinTheta*x + Result.CosTheta*z;
          v3f64 Tester = V3f64(NewX, y, NewZ);
          for(s32 c=0;c<3;c++)
          {
            min.e[c] = fmin(min.e[c], Tester.e[c]);
            max.e[c] = fmax(max.e[c], Tester.e[c]);
          }
        }
      }
    }
    Result.AABB = AABBInit(min, max);
  }
  if(Kind & TransformKind_Translate)
  {
    Result.Offset = Offset;
  }
  return Result;
}
inline constant_medium SurfaceConstantMediumInit(surface *Surface, f64 d, u32 MatId)
{
  //TODO: assert that material from matid is an isotropic
  constant_medium Result = { .Boundary = Surface, .NegInvDensity = d, .MatId = MatId };
  return Result;
}
b32 SurfaceFlipFaceInit(surface *Surface)
{
  Assert(!"Not Implemented");
  return 0;
}


//~ HELPERS
v3f64 OrthoNormBasisGetLocal_3f64(m3f64 Basis, f64 x, f64 y, f64 z)
{
  v3f64 Result = Add(Add(Scale(Basis.u, x),Scale(Basis.v, y)),Scale(Basis.w, z));
  return Result;
}
v3f64 OrthoNormBasisGetLocal_v3f64(m3f64 Basis, v3f64 a)
{
  v3f64 Result = Add(Add(Scale(Basis.u, a.x),Scale(Basis.v, a.y)),Scale(Basis.w, a.z));
  return Result;
}
m3f64 OrthoNormBasisInit(v3f64 u, v3f64 v, v3f64 w)
{
  m3f64 Result = {u,v,w};
  return Result;
}
m3f64 OrthoNormBasisFromNormal(v3f64 n)
{
  m3f64 Result = {0};
  Result.w = Normalize(n);
  v3f64 a = (Abs(Result.w.x)>0.9)?V3f64(0,1,0):V3f64(1,0,0);
  Result.v = Normalize(Cross(Result.w, a));
  Result.u = Normalize(Cross(Result.w, Result.v));
  return Result;
}
inline v3f64 SphereMovingGetPos(sphere_moving *SphereMoving, f64 Time)
{
  v3f64 v = Sub(SphereMoving->Pos1,SphereMoving->Pos0);
  f64   s = ((Time - SphereMoving->Time0)/(SphereMoving->Time1 - SphereMoving->Time0));
  v3f64 Result = Add(SphereMoving->Pos0, Scale(v, s));
  return Result;
}
inline void SphereGetUV(v3f64 Pos, f64 *u, f64 *v)
{
  f64 theta =  acos(-Pos.y);
  f64 phi   = atan2(-Pos.z, Pos.x)+Pi64;
  WriteToRef(u, phi  /Tau64);
  WriteToRef(v, theta/Pi64 );
  return;
}
inline void HitSetFaceNormal(hit_info *Hit, ray Ray, v3f64 OutwardNormal)
{
  Hit->IsFrontFace = Dot(Ray.Dir, OutwardNormal)<0;
  Hit->Normal = Hit->IsFrontFace?OutwardNormal:Scale(OutwardNormal,-1.0);
  return;
}

//~ BOUNCE BEHAVIOR
v3f64 Refract(v3f64 uv, v3f64 Normal, f64 etai_over_etat)
{
  // NOTE(MIGUEL): Abaondoning this function for more complicated stuff in Scatter switch
  f64 CosTheta = Min(Dot(Scale(uv, -1.0), Normal), 1.0);
  
  v3f64 RayOutPerp  = Scale(Add(uv, Scale(Normal, CosTheta)), etai_over_etat);
  v3f64 RayOutParel = Scale(Normal, -SquareRoot(Abs(1.0 - LengthSquared(RayOutPerp))));
  v3f64 Result = Add(RayOutPerp, RayOutParel);
  return Result;
}
f64 Reflectance(f64 Cosine, f64 IndexOfRefraction)
{
  // Use Schlick's approximation
  f64 R0 = (1.0-IndexOfRefraction)/(1.0+IndexOfRefraction);
  R0 = R0*R0;
  f64 Result = R0+(1.0-R0)*Power((1.0-Cosine), 5);
  return Result;
}
f64 SurfaceGetPdfValue(surface *Surface, v3f64 Origin, v3f64 v)
{
  f64 Result = 1.0;
  switch(Surface->Kind)
  {
    case SurfaceKind_RectXZ: {
      hit_info HitInfo = {0};
      if(!SurfaceRectYZHit(&Surface->Rect, &HitInfo, RayInit(Origin, v, 0.0), 0.001, Infintyf64())) 
      {
        return 0.0;
      }
      r3f64 p = Surface->Rect.Points;
      f64 Area = (p.x1 - p.x0)*(p.z1 - p.z0);
      f64 DistSquared = HitInfo.t*HitInfo.t*LengthSquared(v);
      f64 Cosine = Abs(Dot(v, HitInfo.Normal))/Length(v);
      Result = DistSquared/(Cosine*Area);
    } break;
    case SurfaceKind_Sphere: {
      hit_info HitInfo = {0};
      if(!SurfaceSphereHit(&Surface->Sphere, &HitInfo, RayInit(Origin, v, 0.0), 0.001, Infintyf64())) 
      {
        return 0.0;
      }
      f64 SquaredRadius = Surface->Sphere.Radius*Surface->Sphere.Radius;
      f64 SquaredDist = LengthSquared(Sub(Surface->Sphere.Pos,Origin));
      // TODO(MIGUEL): It seems that that it is not reasonable that radius squared would be less than
      //               the squared distance of the ray origin and the sphere pos. So the ratio will always
      //               be greater than one causing the radicand to be negative.
      //               FIGURE OUT THE INTUITION FROM THE BOOK
      f64 Radicand = 1.0-SquaredRadius/SquaredDist;
      Radicand = Radicand<0.0?0.0:Radicand; 
      f64 CosThetaMax = SquareRoot(Radicand);
      f64 SolidAngle  = 2.0*Pi64*(1.0-CosThetaMax);
      Result = 1.0/SolidAngle;
    } break;
    case SurfaceKind_SphereMoving: {
      Result = 1.0;
    } break;
    case SurfaceKind_RectXY: {
      Result = 1.0;
    } break;
    case SurfaceKind_RectYZ: {
      Result = 1.0;
    } break;
    case SurfaceKind_Box: {
      Result = 1.0;
    } break;
    case SurfaceKind_TransformedInst: {
      Result = 1.0;
    } break;
    case SurfaceKind_FlipFace: {
      Result = 1.0;
    } break;
    case SurfaceKind_ConstantMedium: {
      Result = 1.0;
    } break;
    case SurfaceKind_BVHNode: {
      Result = 1.0;
    } break;
  }
  return Result;
}
v3f64 SurfaceGenRandom(surface *Surface, v3f64 Origin)
{
  v3f64 Result = {0};
  switch(Surface->Kind)
  {
    case SurfaceKind_RectXZ: {
      r3f64 p = Surface->Rect.Points;
      v3f64 RandPoint = V3f64(RandF64Range(p.x0, p.x1), Surface->Rect.Offset, RandF64Range(p.z0, p.z1));
      Result = Sub(RandPoint, Origin);
    };
    case SurfaceKind_Sphere: {
      v3f64 Dir =  Sub(Surface->Sphere.Pos, Origin);
      f64   DistSquared = LengthSquared(Dir);
      m3f64 uvw = OrthoNormBasisFromNormal(Dir);
      Result = OrthoNormBasisGetLocal(uvw, RandToSphere(Surface->Sphere.Radius, DistSquared)); 
    };
    case SurfaceKind_SphereMoving: {};
    case SurfaceKind_RectXY: {};
    case SurfaceKind_RectYZ: {};
    case SurfaceKind_Box: {};
    case SurfaceKind_TransformedInst: {};
    case SurfaceKind_FlipFace: {};
    case SurfaceKind_ConstantMedium: {};
    case SurfaceKind_BVHNode: {};
  }
  return Result;
}
f64 MaterialScatterPdf(material *Material, hit_info HitInfo, ray Scattered)
{
  //NOTE: listing 11
  f64 Result = 1.0;
  switch(Material->Kind)
  {
    case MaterialKind_Lambert: {
      f64 Cosine = Dot(HitInfo.Normal, Normalize(Scattered.Dir));
      Result = (Cosine < 0)? 0:Cosine/Pi64;
    }break;
    case MaterialKind_Metal: {
      Result = 1.0;
    }break;
    case MaterialKind_Dielectric: {
      Result = 1.0;
    }break;
    case MaterialKind_DiffuseLight: { 
      Result = 1.0;
    }break;
    case MaterialKind_Isotropic: { 
      Result = 1.0;
    }break;
  }
  
  return Result;
}

b32 MaterialScatter(material *Material, texture *Texture, ray Ray, hit_info *HitInfo, scatter_info *ScatterInfo, scratch *Scratch)
{
  b32 Result = SCATTER_IGNORE;
  switch(Material->Kind)
  {
    case MaterialKind_Lambert:
    {
      pdf *NewPdf = ArenaPushType(Scratch->Arena, pdf);
      WriteToRef(NewPdf, PdfCosineInit(PdfKind_Cosine, HitInfo->Normal));
      ScatterInfo->IsSpecular = 0;
      ScatterInfo->Atten = TextureGetColor(Texture, HitInfo->u, HitInfo->v, HitInfo->Pos);
      ScatterInfo->Pdf = NewPdf;
      Result = SCATTER_PROCESS;
    } break;
    case MaterialKind_Metal:
    {
      v3f64 Reflected = Reflect(Normalize(Ray.Dir), HitInfo->Normal);
      v3f64 Albedo    = TextureGetColor(Texture, HitInfo->u, HitInfo->v, HitInfo->Pos);
      ScatterInfo->SpecularRay = RayInit(HitInfo->Pos, Add(Reflected, Scale(RandInUnitSphere(), Material->Fuzz)), Ray.Time);
      ScatterInfo->Atten = Albedo;
      ScatterInfo->IsSpecular = 1;
      ScatterInfo->Pdf = NULL;
      Result = SCATTER_PROCESS;
    } break;
    case MaterialKind_Dielectric:
    {
      ScatterInfo->IsSpecular = 1;
      ScatterInfo->Pdf = NULL;
      ScatterInfo->Atten = V3f64(1,1,1);
      f64 RefractRatio = HitInfo->IsFrontFace?(1.0/Material->IndexOfRefraction):Material->IndexOfRefraction;
      
      //-
      v3f64 UnitDir   = Normalize(Ray.Dir);
      f64 CosTheta = Min(Dot(Scale(UnitDir, -1.0), HitInfo->Normal), 1.0);
      f64 SinTheta = SquareRoot(1.0 - CosTheta*CosTheta);
      b32 CannotRefract = (RefractRatio*SinTheta>1.0);
      v3f64 Dir = {0};
      if(CannotRefract || Reflectance(CosTheta, RefractRatio)>RandF64Uni()) //should it be randuni or randbi??
      { Dir = Reflect(UnitDir, HitInfo->Normal); }
      else
      { Dir = Refract(UnitDir, HitInfo->Normal, RefractRatio); }
      //-
      ScatterInfo->SpecularRay = RayInit(HitInfo->Pos, Dir, Ray.Time);
      Result = SCATTER_PROCESS;
    } break;
    case MaterialKind_DiffuseLight:
    {
      ScatterInfo->Atten = V3f64(1.0,1.0,1.0);
      Result = SCATTER_IGNORE;
    } break;
    case MaterialKind_Isotropic:
    {
      //WriteToRef(Scattered, RayInit(HitInfo->Pos, RandInUnitSphere(), Ray.Time));
      //WriteToRef(Atten, TextureGetColor(Texture, HitInfo->u,HitInfo->v,HitInfo->Pos));
      Result = SCATTER_PROCESS;
    } break;
  }
  return Result;
}
v3f64 MaterialEmmited(texture *Texture, hit_info Hit)
{
  v3f64 Result = V3f64(0,0,0);
  if(Hit.IsFrontFace)
  {
    Result = Scale(TextureGetColor(Texture, Hit.u, Hit.v, Hit.Pos), 1.0);
  }
  return Result;
}

//~ SURFACE INTERSECTION TESTS
b32 SurfaceSphereHit(sphere *Sphere, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  hit_info NewHit = ObjCopyFromRef(hit_info, Hit);
  // TODO(MIGUEL): Read the sphere interesectio code simplification
  v3f64 oc = Sub(Ray.Origin, Sphere->Pos);
  f64 a = LengthSquared(Ray.Dir);
  f64 half_b = Dot(oc, Ray.Dir);
  f64 c = LengthSquared(oc) - Sphere->Radius*Sphere->Radius;
  f64 Discriminant = half_b*half_b - a*c;
  if (Discriminant<0) { return HIT_NOTDETECTED; }
  f64 sqrtd = SquareRoot(Discriminant);
  
  // Find nearest root
  f64 Root = (-half_b - sqrtd)/a;
  if(Root<Mint || Maxt<Root)
  {
    Root = (-half_b + sqrtd)/a;
    if(Root<Mint || Maxt<Root) { return HIT_NOTDETECTED; }
  }
  NewHit.t   = Root;
  NewHit.Pos = RayAt(Ray, Root);
  v3f64 OutwardNormal = Scale(Sub(NewHit.Pos, Sphere->Pos),1.0/Sphere->Radius);
  HitSetFaceNormal(&NewHit, Ray, OutwardNormal);
  SphereGetUV(OutwardNormal, &NewHit.u, &NewHit.v);
  NewHit.MatId = Sphere->MatId;
  WriteToRef(Hit, NewHit); 
  return HIT_DETECTED;
  
}
b32 SurfaceSphereMovingHit(sphere_moving *Sphere, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  hit_info NewHit = ObjCopyFromRef(hit_info, Hit);
  // TODO(MIGUEL): Read the sphere interesectio code simplification
  v3f64 oc = Sub(Ray.Origin, SphereMovingGetPos(Sphere, Ray.Time));
  f64 a = LengthSquared(Ray.Dir);
  f64 half_b = Dot(oc, Ray.Dir);
  f64 c = LengthSquared(oc) - Sphere->Radius*Sphere->Radius;
  f64 Discriminant = half_b*half_b - a*c;
  if (Discriminant<0) { return HIT_NOTDETECTED; }
  f64 sqrtd = SquareRoot(Discriminant);
  
  // Find nearest root
  f64 Root = (-half_b - sqrtd)/a;
  if(Root<Mint || Maxt<Root)
  {
    Root = (-half_b + sqrtd)/a;
    if(Root<Mint || Maxt<Root) { return HIT_NOTDETECTED; }
  }
  NewHit.t   = Root;
  NewHit.Pos = RayAt(Ray, Root);
  v3f64 OutwardNormal = Scale(Sub(NewHit.Pos, SphereMovingGetPos(Sphere, Ray.Time)),1.0/Sphere->Radius);
  HitSetFaceNormal(&NewHit, Ray, OutwardNormal);
  SphereGetUV(OutwardNormal, &NewHit.u, &NewHit.v);
  NewHit.MatId = Sphere->MatId;
  WriteToRef(Hit, NewHit); 
  return HIT_DETECTED;
}
b32 SurfaceAABBHit(aabb *Aabb, ray Ray, f64 tmin, f64 tmax)
{
  for(int a=0;a<3;a++)
  {
    f64 t0 = fmin((Aabb->min.e[a] - Ray.Origin.e[a])/Ray.Dir.e[a],
                  (Aabb->max.e[a] - Ray.Origin.e[a])/Ray.Dir.e[a]);
    f64 t1 = fmax((Aabb->min.e[a] - Ray.Origin.e[a])/Ray.Dir.e[a],
                  (Aabb->max.e[a] - Ray.Origin.e[a])/Ray.Dir.e[a]);
    tmin = fmax(t0, tmin);
    tmax = fmin(t1, tmax);
    if(tmax <= tmin) return HIT_NOTDETECTED;
  }
  return HIT_DETECTED;
}
b32 SurfaceBVHNodeHit(bvh_node *BvhNode, hit_info *HitInfo, ray Ray, f64 tmin, f64 tmax)
{
  if(!SurfaceAABBHit(&BvhNode->AABB, Ray, tmin, tmax)) return HIT_NOTDETECTED;
  b32 hit_left  = SurfaceHit(BvhNode->Left , HitInfo, Ray, tmin, tmax);
  b32 hit_right = SurfaceHit(BvhNode->Right, HitInfo, Ray, tmin, hit_left?HitInfo->t:tmax);
  return (hit_left || hit_right);
}
b32 SurfaceRectXYHit(rect *Rect, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  hit_info NewHitInfo = ObjCopyFromRef(hit_info, HitInfo);
  r3f64 p = Rect->Points;
  f64 t = (Rect->Offset - Ray.Origin.z)/Ray.Dir.z;
  if(t < Mint || t > Maxt) return HIT_NOTDETECTED;
  
  f64 x = Ray.Origin.x + t*Ray.Dir.x;
  f64 y = Ray.Origin.y + t*Ray.Dir.y;
  if(x < p.x0 || x > p.x1 || y < p.y0 || y > p.y1) return HIT_NOTDETECTED;
  
  NewHitInfo.u = (x - p.x0)/(p.x1-p.x0);
  NewHitInfo.v = (y - p.y0)/(p.y1-p.y0);
  NewHitInfo.t = t;
  v3f64 OutwardNormal = V3f64(0,0,1);
  HitSetFaceNormal(&NewHitInfo, Ray, OutwardNormal);
  NewHitInfo.MatId = Rect->MatId;
  NewHitInfo.Pos = RayAt(Ray, t);
  WriteToRef(HitInfo, NewHitInfo);
  return HIT_DETECTED;
}
b32 SurfaceRectXZHit(rect *Rect, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  hit_info NewHitInfo = ObjCopyFromRef(hit_info, HitInfo);
  r3f64 p = Rect->Points;
  f64 t = (Rect->Offset - Ray.Origin.y)/Ray.Dir.y;
  if(t < Mint || t > Maxt) return HIT_NOTDETECTED;
  
  f64 x = Ray.Origin.x + t*Ray.Dir.x;
  f64 z = Ray.Origin.z + t*Ray.Dir.z;
  if(x < p.x0 || x > p.x1 || z < p.z0 || z > p.z1) return HIT_NOTDETECTED;
  
  NewHitInfo.u = (x - p.x0)/(p.x1-p.x0);
  NewHitInfo.v = (z - p.z0)/(p.z1-p.z0);
  NewHitInfo.t = t;
  v3f64 OutwardNormal = V3f64(0,1,0);
  HitSetFaceNormal(&NewHitInfo, Ray, OutwardNormal);
  NewHitInfo.MatId = Rect->MatId;
  NewHitInfo.Pos = RayAt(Ray, t);
  WriteToRef(HitInfo, NewHitInfo);
  return HIT_DETECTED;
}
b32 SurfaceRectYZHit(rect *Rect, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  hit_info NewHitInfo = ObjCopyFromRef(hit_info, HitInfo);
  r3f64 p = Rect->Points;
  f64 t = (Rect->Offset - Ray.Origin.x)/Ray.Dir.x;
  if(t < Mint || t > Maxt) return HIT_NOTDETECTED;
  
  f64 y = Ray.Origin.y + t*Ray.Dir.y;
  f64 z = Ray.Origin.z + t*Ray.Dir.z;
  if(y < p.y0 || y > p.y1 || z < p.z0 || z > p.z1) return HIT_NOTDETECTED;
  
  NewHitInfo.u = (y - p.y0)/(p.y1-p.y0);
  NewHitInfo.v = (z - p.z0)/(p.z1-p.z0);
  NewHitInfo.t = t;
  v3f64 OutwardNormal = V3f64(1,0,0);
  HitSetFaceNormal(&NewHitInfo, Ray, OutwardNormal);
  NewHitInfo.MatId = Rect->MatId;
  NewHitInfo.Pos = RayAt(Ray, t);
  WriteToRef(HitInfo, NewHitInfo);
  return HIT_DETECTED;
}
b32 SurfaceBoxHit(box *Box, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  //// TODO(MIGUEL): Test AABB first
  b32 Result = SurfaceListHit(Box->SidesList, 6, HitInfo, Ray, Mint, Maxt);
  return Result;
}
b32 SurfaceTransformedInstHit(transformed_inst *Instance, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  if(Instance->Kind == (TransformKind_Translate | TransformKind_RotateY))
  {
    ray MovedRay = RayInit(Sub(Ray.Origin, Instance->Offset), Ray.Dir, Ray.Time);
    v3f64 Origin = MovedRay.Origin;
    v3f64 Dir    = MovedRay.Dir;
    f64 SinTheta = Instance->SinTheta;
    f64 CosTheta = Instance->CosTheta;
    Origin.x = CosTheta*MovedRay.Origin.x - SinTheta*MovedRay.Origin.z;
    Origin.z = SinTheta*MovedRay.Origin.x + CosTheta*MovedRay.Origin.z;
    Dir.x    = CosTheta*MovedRay.Dir.x    - SinTheta*MovedRay.Dir.z;
    Dir.z    = SinTheta*MovedRay.Dir.x    + CosTheta*MovedRay.Dir.z;
    
    ray RotatedRay = RayInit(Origin, Dir, MovedRay.Time);
    if(!SurfaceHit(Instance->Instance, HitInfo, RotatedRay, Mint, Maxt)) 
    {
      return HIT_NOTDETECTED;
    }
    
    v3f64 Pos    = HitInfo->Pos;
    v3f64 Normal = HitInfo->Normal;
    
    Pos.x    =  CosTheta*HitInfo->Pos.x    + SinTheta*HitInfo->Pos.z;
    Pos.z    = -SinTheta*HitInfo->Pos.x    + CosTheta*HitInfo->Pos.z;
    Normal.x =  CosTheta*HitInfo->Normal.x + SinTheta*HitInfo->Normal.z;
    Normal.z = -SinTheta*HitInfo->Normal.x + CosTheta*HitInfo->Normal.z;
    
    HitInfo->Pos = Pos;
    HitSetFaceNormal(HitInfo, RotatedRay, Normal);
    HitInfo->Pos = Add(HitInfo->Pos, Instance->Offset);
    HitSetFaceNormal(HitInfo, MovedRay, HitInfo->Normal);
    return HIT_DETECTED;
  }
  if(Instance->Kind == TransformKind_RotateY)
  {
    v3f64 Origin = Ray.Origin;
    v3f64 Dir    = Ray.Dir;
    f64 SinTheta = Instance->SinTheta;
    f64 CosTheta = Instance->CosTheta;
    Origin.x = CosTheta*Ray.Origin.x - SinTheta*Ray.Origin.z;
    Origin.z = SinTheta*Ray.Origin.x + CosTheta*Ray.Origin.z;
    Dir.x    = CosTheta*Ray.Dir.x    - SinTheta*Ray.Dir.z;
    Dir.z    = SinTheta*Ray.Dir.x    + CosTheta*Ray.Dir.z;
    
    ray RotatedRay = RayInit(Origin, Dir, Ray.Time);
    if(!SurfaceHit(Instance->Instance, HitInfo, RotatedRay, Mint, Maxt)) 
    {
      return HIT_NOTDETECTED;
    }
    
    v3f64 Pos    = HitInfo->Pos;
    v3f64 Normal = HitInfo->Normal;
    
    Pos.x    =  CosTheta*HitInfo->Pos.x    + SinTheta*HitInfo->Pos.z;
    Pos.z    = -SinTheta*HitInfo->Pos.x    + CosTheta*HitInfo->Pos.z;
    Normal.x =  CosTheta*HitInfo->Normal.x + SinTheta*HitInfo->Normal.z;
    Normal.z = -SinTheta*HitInfo->Normal.x + CosTheta*HitInfo->Normal.z;
    
    HitInfo->Pos = Pos;
    HitSetFaceNormal(HitInfo, RotatedRay, Normal);
  }
  if(Instance->Kind == TransformKind_Translate)
  {
    ray MovedRay = RayInit(Sub(Ray.Origin, Instance->Offset), Ray.Dir, Ray.Time);
    if(!SurfaceHit(Instance->Instance, HitInfo, MovedRay, Mint, Maxt))
    { 
      return HIT_NOTDETECTED;
    }
    HitInfo->Pos = Add(HitInfo->Pos, Instance->Offset);
    HitSetFaceNormal(HitInfo, MovedRay, HitInfo->Normal);
  }
  return HIT_DETECTED;
}
b32 SurfaceConstantMediumHit(constant_medium *Medium, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  b32 EnableDebug = 0;
  b32 Debugging   = EnableDebug && (RandF64Uni()<0.00001);
  hit_info HitInfo1 = {0};
  hit_info HitInfo2 = {0};
  
  if(!SurfaceHit(Medium->Boundary, &HitInfo1, Ray, -Infintyf64(), Infintyf64())) { return HIT_NOTDETECTED; }
  if(!SurfaceHit(Medium->Boundary, &HitInfo2, Ray,  HitInfo1.t+0.001, Infintyf64())) { return HIT_NOTDETECTED; }
  if(Debugging) fprintf(stderr, "\nMint=%f, Maxt=%f\n", HitInfo1.t, HitInfo2.t);
  if(HitInfo1.t < Mint) HitInfo1.t = Mint;
  if(HitInfo2.t > Maxt) HitInfo2.t = Maxt;
  
  if(HitInfo1.t >= HitInfo2.t) return HIT_NOTDETECTED;
  
  if(HitInfo1.t < 0.0) HitInfo1.t = 0.0;
  
  f64 RayLength = Length(Ray.Dir);
  f64 DistInBoundary = (HitInfo2.t - HitInfo1.t)*RayLength;
  f64 HitDist = Medium->NegInvDensity*Log(RandF64Uni());
  if(HitDist>DistInBoundary) { return HIT_NOTDETECTED; }
  
  HitInfo->t   = HitInfo1.t + HitDist/RayLength;
  HitInfo->Pos = RayAt(Ray, HitInfo->t);
  
  if(Debugging)
  {
    fprintf(stderr,
            "HitInfo Distnace = %f\n"
            "HitInfo.t = %f\n"
            "HitInfo.p = (%f,%f,%f)\n", 
            HitDist, HitInfo->t,
            HitInfo->Pos.x, HitInfo->Pos.y, HitInfo->Pos.z);
  }
  HitInfo->Normal = V3f64(1,0,0);
  HitInfo->IsFrontFace = 1;
  HitInfo->MatId = Medium->MatId;
  
  return HIT_DETECTED;
}
b32 SurfaceFlipFaceHit(flip_face *FlipFace, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  // listing 21
  // NOTE(MIGUEL): Having an object just to perform an operation like this seems kind of silly...
  //               But ill do this to not stray from the tutorial
  if(FlipFace->Surface==NULL) Assert(!"Bad refernce managment detected");
  if(!SurfaceHit(FlipFace->Surface, HitInfo, Ray, Mint, Maxt)) return HIT_NOTDETECTED;
  HitInfo->IsFrontFace = !HitInfo->IsFrontFace;
  return HIT_DETECTED;
}
b32 SurfaceHit(surface *Surface, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  b32 Result = 0;
  switch(Surface->Kind)
  {
    case SurfaceKind_Sphere: {
      Result = SurfaceSphereHit(&Surface->Sphere, HitInfo, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_SphereMoving: {
      Result = SurfaceSphereMovingHit(&Surface->SphereMoving, HitInfo, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_RectXY: {
      Result = SurfaceRectXYHit(&Surface->Rect, HitInfo, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_RectXZ: {
      Result = SurfaceRectXZHit(&Surface->Rect, HitInfo, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_RectYZ: {
      Result = SurfaceRectYZHit(&Surface->Rect, HitInfo, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_Box: {
      Result = SurfaceBoxHit(&Surface->Box, HitInfo, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_TransformedInst: {
      Result = SurfaceTransformedInstHit(&Surface->TransformedInst, HitInfo, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_FlipFace: { 
      Result = SurfaceFlipFaceHit(&Surface->FlipFace, HitInfo, Ray, Mint, Maxt);
    } break;
    case SurfaceKind_ConstantMedium: { 
      Result = SurfaceConstantMediumHit(&Surface->ConstantMedium, HitInfo, Ray, Mint, Maxt);
    } break;
    case SurfaceKind_BVHNode: {
      Result = SurfaceBVHNodeHit(&Surface->BvhNode, HitInfo, Ray, Mint, Maxt);
    }break;
  }
  return Result;
}
b32 SurfaceListHit(surface *Surfaces, u32 SurfaceCount, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  b32 SurfaceWasHit = 0;
  f64 ClosestHit = Maxt;
  hit_info TempHitInfo = {0};
  
  for(u32 SurfaceId=0; SurfaceId<SurfaceCount; SurfaceId++)
  {
    if(SurfaceHit(&Surfaces[SurfaceId], &TempHitInfo, Ray, Mint, ClosestHit))
    {
      SurfaceWasHit = 1;
      ClosestHit = TempHitInfo.t;
      WriteToRef(HitInfo, TempHitInfo);
    }
  }
  return SurfaceWasHit;
}
b32 SurfaceBVHListHit(surface *BVHList, u32 BVHCount, hit_info *HitInfo, ray Ray, f64 Mint, f64 Maxt)
{
  b32 SurfaceWasHit = 0;
  f64 ClosestHit = Maxt;
  hit_info TempHitInfo = {0};
  for(u32 BVHId=0; BVHId<BVHCount; BVHId++)
  {
    if(SurfaceHit(&BVHList[BVHId], &TempHitInfo, Ray, Mint, ClosestHit))
    {
      SurfaceWasHit = 1;
      ClosestHit = TempHitInfo.t;
      WriteToRef(HitInfo, TempHitInfo);
    }
  }
  return SurfaceWasHit;
}