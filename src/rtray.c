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
  if(Depth<=0) { return V3f64(0,0,0); }
  if(!WorldHit(World, &Hit, Ray, 0.001, Infintyf64(), 1)) { return World->DefaultBackground; }
  
  ray ScatteredRay = {0};
  //v3f64 Atten   = {0};  // deprecated
  material *Mat = WorldMaterialGetFromId(World, Hit.MatId);
  texture  *Tex = WorldTextureGetFromId(World, Mat->TexId);
  v3f64     Emmited = MaterialEmmited(Mat, Tex, Hit.u, Hit.v, Hit.Pos);
  f64       Pdf = 0.0;
  v3f64     Albedo = {0};
  if(!MaterialScatter(Mat, Tex, Ray, Hit, &Albedo, &ScatteredRay, &Pdf))
  {
    //NOTE: for dark scenes having a low emmision color values is making the image black when
    //      the resolution is high. It seems like increasing the color emmision values help.
    //      It seems the if a lot of the scene background is black and rays samples have a high chance of 
    //      not tracing to an emitting obj then the average will be swayed greatly towwrds the background
    //      color.
    //      suspected issues:
    //      - not generating scattered rays correctly
    //      It seems like doing a bidieraction x,y offset of the pixel coord helped the quality slightly but did
    //      not solve the issue. Instead I noticed that the apeture value was set to zero and so the ray given by the
    //      camera was not altered by the random unit disk offset.
    //      Setting the apeture to a positive value seemed to have fixed the issue.
    return Emmited;
  }
  
  //LIGHT SAMPLING
  // listing 19
  v3f64 OnLight = V3f64(RandF64Range(213.0, 343.0), 554.0, RandF64Range(227.0, 332.0));
  v3f64 ToLight = Sub(OnLight, Hit.Pos);
  f64 DistSquared = LengthSquared(ToLight);
  ToLight =  Normalize(ToLight);
  
  if(Dot(ToLight, Hit.Normal)<0) return Emmited;
  f64 LightArea = (343.0-213.0)*(332.0-227.0);
  f64 LightCosine = Abs(ToLight.y);
  if(LightCosine<0.0000001) return Emmited;
  
  Pdf = DistSquared/(LightCosine*LightArea);
  ScatteredRay = RayInit(Hit.Pos, ToLight, Ray.Time);
  //LIGHT SAMPLING
  
  v3f64 NextRayColor = RayColor(ScatteredRay, Depth - 1, World);
  f64   ScatterPdf   = MaterialScatterPdf(Mat, Ray, Hit, ScatteredRay);
  v3f64 Result       = Add(Emmited, Scale(Mul(Albedo, NextRayColor), ScatterPdf/Pdf));
  return Result;
}