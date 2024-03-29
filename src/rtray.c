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
v3f64 RayColor(ray Ray, s32 Depth, world *World, scratch *Scratch)
{
  if(Depth<=0) { return V3f64(0,0,0); }
  
  hit_info     HitInfo = {0};
  scatter_info ScatterInfo = {0};
  
  f64 Biasedt = 0.5 + 0.5*Ray.Dir.y;
  v3f64 Background = Lerp(World->DefaultBackground[0], World->DefaultBackground[1], Biasedt);
  surface *Surface = WorldHit(World, &HitInfo, Ray, 0.001, Infintyf64(), RT_TRUE);
  if(!Surface) { return Background; }
  
  material *Mat     = WorldMaterialGetFromId(World, HitInfo.MatId);
  texture  *Tex     = WorldTextureGetFromId(World, Mat->TexId);
  
  v3f64     Emmited = MaterialEmmited(Tex, HitInfo);
  if(!MaterialScatter(Mat, Tex, Ray, &HitInfo, &ScatterInfo, Scratch))
  {
    return Emmited;
  }
  if(ScatterInfo.IsSpecular)
  {
    v3f64 Specular = Mul(ScatterInfo.Atten, RayColor(ScatterInfo.SpecularRay, Depth-1, World, Scratch));
    return Specular;
  }
  
  // PDF listing 31
  pdf PdfA   = {0};//ArenaPushType(Scratch->Arena, pdf);
  pdf PdfB   = {0};//ArenaPushType(Scratch->Arena, pdf);
  pdf PdfMix = {0};//ArenaPushType(Scratch->Arena, pdf);
  WriteToRef(&PdfA  , PdfInit(PdfKind_Surface, NULL, NULL, World->Lights[0].Surface, HitInfo.Pos, HitInfo.Normal));
  WriteToRef(&PdfB  , PdfInit(PdfKind_Cosine, NULL, NULL, NULL, V3f64(0,0,0), HitInfo.Normal));
  WriteToRef(&PdfMix, PdfInit(PdfKind_Mixture, &PdfA, &PdfB, NULL, V3f64(0,0,0), V3f64(0,0,0)));
  
  ray ScatteredRay = RayInit(HitInfo.Pos, PdfMixtureGenerate(&PdfMix), Ray.Time);
  f64 PdfVal       = PdfGetValue(&PdfMix, ScatteredRay.Dir);
  
  //Assert(!((-0.00001 <= PdfVal) && (PdfVal<=0.00001)));
  
  v3f64 NextRayColor  = RayColor(ScatteredRay, Depth - 1, World, Scratch); 
  f64   ScatterPdfVal = MaterialScatterPdf(Mat, HitInfo, ScatteredRay);
  v3f64 Result        = Add(Emmited, Scale(Mul(ScatterInfo.Atten, NextRayColor), ScatterPdfVal/(PdfVal+0.0001)));
  //Assert(!HasNanV3(Result));
  return Result;
}