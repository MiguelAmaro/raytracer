
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

//~ HELPERS
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
inline void HitSetFaceNormal(hit *Hit, ray Ray, v3f64 OutwardNormal)
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
b32 MaterialScatter(material *Material, texture *Texture, ray Ray, hit Hit, v3f64 *Atten, ray *Scattered)
{
  b32 Result = SCATTER_IGNORE;
  switch(Material->Kind)
  {
    case MaterialKind_Lambert:
    {
      v3f64 ScatterDir = Add(Hit.Normal, RandUnitVector());
      if(NearZero(ScatterDir)) { ScatterDir = Hit.Normal; }
      
      WriteToRef(Scattered, RayInit(Hit.Pos, ScatterDir, Ray.Time));
      WriteToRef(Atten, TextureGetColor(Texture, Hit.u, Hit.v, Hit.Pos));
      Result = SCATTER_PROCESS;
    } break;
    case MaterialKind_Metal:
    {
      v3f64 Reflected = Reflect(Normalize(Ray.Dir), Hit.Normal);
      WriteToRef(Scattered, RayInit(Hit.Pos, Add(Reflected, Scale(RandInUnitSphere(), Material->Fuzz)), Ray.Time));
      WriteToRef(Atten, TextureGetColor(Texture, Hit.u, Hit.v, Hit.Pos));
      Result = (Dot(Scattered->Dir, Hit.Normal)>0)?SCATTER_PROCESS:SCATTER_IGNORE;
    } break;
    case MaterialKind_Dielectric:
    {
      WriteToRef(Atten, V3f64(1.0,1.0,1.0));
      f64 RefractRatio = Hit.IsFrontFace?(1.0/Material->IndexOfRefraction):Material->IndexOfRefraction;
      v3f64 UnitDir   = Normalize(Ray.Dir);
      f64 CosTheta = Min(Dot(Scale(UnitDir, -1.0), Hit.Normal), 1.0);
      f64 SinTheta = SquareRoot(1.0 - CosTheta*CosTheta);
      b32 CannotRefract = (RefractRatio*SinTheta>1.0);
      v3f64 Dir = {0};
      if(CannotRefract || Reflectance(CosTheta, RefractRatio)>RandF64Uni()) //should it be randuni or randbi
      { Dir = Reflect(UnitDir, Hit.Normal); }
      else
      { Dir = Refract(UnitDir, Hit.Normal, RefractRatio); }
      WriteToRef(Scattered, RayInit(Hit.Pos, Dir, Ray.Time));
      Result = SCATTER_PROCESS;
    } break;
    case MaterialKind_DiffuseLight:
    {
      WriteToRef(Atten, V3f64(1.0,1.0,1.0));
      Result = SCATTER_IGNORE;
    } break;
  }
  return Result;
}
v3f64 MaterialEmmited(material *Material, texture *Texture, f64 u, f64 v, v3f64 Pos)
{
  v3f64 Result = V3f64(0,0,0);
  if(Material->Kind==MaterialKind_DiffuseLight)
  {
    Result = Scale(TextureGetColor(Texture, u, v, Pos), 1.0);
  }
  return Result;
}

//~ SURFACE INTERSECTION TESTS
b32 SurfaceSphereHit(sphere *Sphere, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  hit NewHit = {0};
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
b32 SurfaceSphereMovingHit(sphere_moving *Sphere, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  hit NewHit = {0};
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
b32 AABBHit(aabb *Aabb, ray Ray, f64 tmin, f64 tmax)
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
b32 SurfaceBVHNodeHit(bvh_node *BvhNode, hit *Hit, ray Ray, f64 tmin, f64 tmax)
{
  if(!AABBHit(&BvhNode->AABB, Ray, tmin, tmax)) return HIT_NOTDETECTED;
  b32 hit_left  = SurfaceHit(BvhNode->Left, Hit, Ray, tmin, tmax);
  b32 hit_right = SurfaceHit(BvhNode->Right, Hit, Ray, tmin, hit_left?Hit->t:tmax);
  return (hit_left || hit_right);
}
#if 0
b32 SurfacePlaneHit(plane *Plane, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  return HIT_DETECTED;
}
#endif
b32 SurfaceRectXYHit(rect *Rect, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  hit NewHit = {0};
  r3f64 p = Rect->Points;
  f64 t = (Rect->Offset - Ray.Origin.z)/Ray.Dir.z;
  if(t < Mint || t > Maxt) return HIT_NOTDETECTED;
  
  f64 x = Ray.Origin.x + t*Ray.Dir.x;
  f64 y = Ray.Origin.y + t*Ray.Dir.y;
  if(x < p.x0 || x > p.x1 || y < p.y0 || y > p.y1) return HIT_NOTDETECTED;
  
  NewHit.u = (x - p.x0)/(p.x1-p.x0);
  NewHit.v = (y - p.y0)/(p.y1-p.y0);
  NewHit.t = t;
  v3f64 OutwardNormal = V3f64(0,0,1);
  HitSetFaceNormal(&NewHit, Ray, OutwardNormal);
  NewHit.MatId = Rect->MatId;
  NewHit.Pos = RayAt(Ray, t);
  WriteToRef(Hit, NewHit);
  return HIT_DETECTED;
}
b32 SurfaceRectXZHit(rect *Rect, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  hit NewHit = {0};
  r3f64 p = Rect->Points;
  f64 t = (Rect->Offset - Ray.Origin.y)/Ray.Dir.y;
  if(t < Mint || t > Maxt) return HIT_NOTDETECTED;
  
  f64 x = Ray.Origin.x + t*Ray.Dir.x;
  f64 z = Ray.Origin.z + t*Ray.Dir.z;
  if(x < p.x0 || x > p.x1 || z < p.z0 || z > p.z1) return HIT_NOTDETECTED;
  
  NewHit.u = (x - p.x0)/(p.x1-p.x0);
  NewHit.v = (z - p.z0)/(p.z1-p.z0);
  NewHit.t = t;
  v3f64 OutwardNormal = V3f64(0,1,0);
  HitSetFaceNormal(&NewHit, Ray, OutwardNormal);
  NewHit.MatId = Rect->MatId;
  NewHit.Pos = RayAt(Ray, t);
  WriteToRef(Hit, NewHit);
  return HIT_DETECTED;
}
b32 SurfaceRectYZHit(rect *Rect, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  hit NewHit = {0};
  r3f64 p = Rect->Points;
  f64 t = (Rect->Offset - Ray.Origin.x)/Ray.Dir.x;
  if(t < Mint || t > Maxt) return HIT_NOTDETECTED;
  
  f64 y = Ray.Origin.y + t*Ray.Dir.y;
  f64 z = Ray.Origin.z + t*Ray.Dir.z;
  if(y < p.y0 || y > p.y1 || z < p.z0 || z > p.z1) return HIT_NOTDETECTED;
  
  NewHit.u = (y - p.y0)/(p.y1-p.y0);
  NewHit.v = (z - p.z0)/(p.z1-p.z0);
  NewHit.t = t;
  v3f64 OutwardNormal = V3f64(1,0,0);
  HitSetFaceNormal(&NewHit, Ray, OutwardNormal);
  NewHit.MatId = Rect->MatId;
  NewHit.Pos = RayAt(Ray, t);
  WriteToRef(Hit, NewHit);
  return HIT_DETECTED;
}
b32 SurfaceHit(surface *Surface, hit *Hit, ray Ray, f64 Mint, f64 Maxt)
{
  b32 Result = 0;
  switch(Surface->Kind)
  {
    case SurfaceKind_Sphere: {
      Result = SurfaceSphereHit(&Surface->Sphere, Hit, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_SphereMoving: {
      Result = SurfaceSphereMovingHit(&Surface->SphereMoving, Hit, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_RectXY: {
      Result = SurfaceRectXYHit(&Surface->Rect, Hit, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_RectXZ: {
      Result = SurfaceRectXZHit(&Surface->Rect, Hit, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_RectYZ: {
      Result = SurfaceRectYZHit(&Surface->Rect, Hit, Ray, Mint, Maxt);
    }break;
    case SurfaceKind_BVHNode: {
      Result = SurfaceBVHNodeHit(&Surface->BvhNode, Hit, Ray, Mint, Maxt);
    }break;
  }
  return Result;
}