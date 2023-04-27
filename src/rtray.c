ray RayInit(v3f64 Origin, v3f64 Dir, f64 Time)
{
  ray r = {.Origin=Origin, .Dir=Dir, .Time=Time};
  return r;
}
v3f64 RayAt(ray Ray, f64 t)
{
  v3f64 r = Add(Ray.Origin, Scale(Ray.Dir, t));
  return r;
}
v3f64 RayColor(ray Ray, s32 Depth, world *World)
{
  hit Hit = {0};
  if(Depth<=0) { return V3f64(0.0,0.0,0.0); }
  if(WorldHitBVH(World, &Hit, Ray, 0.001, Inifintyf64()))
  {
    ray Scattered = {0};
    v3f64 Atten   = {0};
    material *Mat = WorldMaterialGetFromId(World, Hit.MatId);
    texture  *Tex = WorldTextureGetFromId(World, Mat->TexId);
    if(MaterialScatter(Mat, Tex, Ray, Hit, &Atten, &Scattered))
    {
      return Mul(Atten, RayColor(Scattered, Depth - 1, World));
    }
    return V3f64(0.0,0.0,0.0);
  }
  v3f64 UnitDir = Normalize(Ray.Dir);
  f64 t = 0.5*(UnitDir.y+1.0);
  v3f64 r = Lerp(V3f64(1.0,1.0,1.0), V3f64(0.5,0.7,1.0), t);
  return r;
}