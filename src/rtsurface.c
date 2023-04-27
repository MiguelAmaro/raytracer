
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
  f64 theta = acos(-Pos.y);
  f64 phi   = atan2(-Pos.z, Pos.x+Pi64);
  WriteToRef(u, phi  /(2.0*Pi64));
  WriteToRef(v, theta/(1.0*Pi64));
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
  v3f64 RayOutParel = Scale(Normal, -SquareRoot(fabs(1.0 - LengthSquared(RayOutPerp))));
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
  b32 Result = 0;
  switch(Material->Kind)
  {
    case MaterialKind_Lambert:
    {
      v3f64 ScatterDir = Add(Hit.Normal, RandUnitVector());
      if(NearZero(ScatterDir)) { ScatterDir = Hit.Normal; }
      
      WriteToRef(Scattered, RayInit(Hit.Pos, ScatterDir, Ray.Time));
      WriteToRef(Atten, Texture->Color);
      Result = 1;
    } break;
    case MaterialKind_Metal:
    {
      v3f64 Reflected = Reflect(Normalize(Ray.Dir), Hit.Normal);
      WriteToRef(Scattered, RayInit(Hit.Pos, Add(Reflected, Scale(RandInUnitSphere(), Material->Fuzz)), Ray.Time));
      WriteToRef(Atten, Texture->Color);
      Result = (Dot(Scattered->Dir, Hit.Normal)>0);
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
      Result = 1;
    } break;
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
  if (Discriminant<0) { return 0; }
  f64 sqrtd = SquareRoot(Discriminant);
  
  // Find nearest root
  f64 Root = (-half_b - sqrtd)/a;
  if(Root<Mint || Maxt<Root)
  {
    Root = (-half_b + sqrtd)/a;
    if(Root<Mint || Maxt<Root) { return 0; }
  }
  NewHit.t   = Root;
  NewHit.Pos = RayAt(Ray, Root);
  v3f64 OutwardNormal = Scale(Sub(NewHit.Pos, Sphere->Pos),1.0/Sphere->Radius);
  HitSetFaceNormal(&NewHit, Ray, OutwardNormal);
  SphereGetUV(OutwardNormal, &Hit->u, &Hit->v);
  NewHit.MatId = Sphere->MatId;
  WriteToRef(Hit, NewHit); 
  return 1;
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
  if (Discriminant<0) { return 0; }
  f64 sqrtd = SquareRoot(Discriminant);
  
  // Find nearest root
  f64 Root = (-half_b - sqrtd)/a;
  if(Root<Mint || Maxt<Root)
  {
    Root = (-half_b + sqrtd)/a;
    if(Root<Mint || Maxt<Root) { return 0; }
  }
  NewHit.t   = Root;
  NewHit.Pos = RayAt(Ray, Root);
  v3f64 OutwardNormal = Scale(Sub(NewHit.Pos, SphereMovingGetPos(Sphere, Ray.Time)),1.0/Sphere->Radius);
  HitSetFaceNormal(&NewHit, Ray, OutwardNormal);
  NewHit.MatId = Sphere->MatId;
  WriteToRef(Hit, NewHit); 
  return 1;
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
    if(tmax <= tmin) return 0;
  }
  return 1;
}
b32 SurfaceHit(surface *Surface, hit *Hit, ray Ray, f64 Mint, f64 Maxt);
b32 SurfaceBVHNodeHit(bvh_node *BvhNode, hit *Hit, ray Ray, f64 tmin, f64 tmax)
{
  if(!AABBHit(&BvhNode->AABB, Ray, tmin, tmax)) return 0;
  b32 hit_left  = SurfaceHit(BvhNode->Left, Hit, Ray, tmin, tmax);
  b32 hit_right = SurfaceHit(BvhNode->Right, Hit, Ray, tmin, hit_left?Hit->t:tmax);
  return (hit_left || hit_right);
}
//b32 SurfacePlaneHit(plane *Plane, hit *Hit, ray Ray, f64 Mint, f64 Maxt) {return 1;}
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
    case SurfaceKind_Plane: {
      //SurfacePlaneMovingHit();
    }break;
    case SurfaceKind_BVHNode: {
      Result = SurfaceBVHNodeHit(&Surface->BvhNode, Hit, Ray, Mint, Maxt);
    }break;
  }
  return Result;;
}