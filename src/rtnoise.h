#ifndef RTNOISE_H
#define RTNOISE_H

typedef enum noise_kind noise_kind;
enum noise_kind
{
  NoiseKind_Perin,
};
#define NOISE_POINTCOUNT (256)
typedef struct noise noise;
struct noise 
{
  noise_kind Kind;
  v3f64 Samples[NOISE_POINTCOUNT];
  s32 PermX[NOISE_POINTCOUNT];
  s32 PermY[NOISE_POINTCOUNT];
  s32 PermZ[NOISE_POINTCOUNT];
};

void NoisePermGen(s32 *Perms, u32 PermCount)
{
  for(u32 i=0;i<PermCount;i++) { Perms[i] = i; }
  for(int i=PermCount-1;i>0;i--)
  {
    s32 target = RandInt(0, i);
    s32 tmp = Perms[i];
    Perms[i] = Perms[target];
    Perms[target] = tmp;
  }
}
noise NoiseInit(noise_kind Kind)
{
  noise Result = { .Kind = Kind};
  switch(Kind)
  {
    case NoiseKind_Perin: {
      for(u32 i=0;i<NOISE_POINTCOUNT;i++) { Result.Samples[i] = RandUnitVector(); }
      NoisePermGen(Result.PermX, NOISE_POINTCOUNT);
      NoisePermGen(Result.PermY, NOISE_POINTCOUNT);
      NoisePermGen(Result.PermZ, NOISE_POINTCOUNT);
    }break;
  }
  return Result;
}
f64 NoiseGetValueSmooth(noise *Noise, v3f64 Pos)
{
  f64 u = Pos.x - Floor(Pos.x);
  f64 v = Pos.y - Floor(Pos.y);
  f64 w = Pos.z - Floor(Pos.z);
  
  s32 i = (s32)Floor(Pos.x);
  s32 j = (s32)Floor(Pos.y);
  s32 k = (s32)Floor(Pos.z);
  v3f64 c[2][2][2] = {0};
  
  for(int di=0;di<2;di++) {
    for(int dj=0;dj<2;dj++) {
      for(int dk=0;dk<2;dk++) {
        c[di][dj][dk] = Noise->Samples[Noise->PermX[(i+di)&(NOISE_POINTCOUNT-1)] ^
                                       Noise->PermX[(j+dj)&(NOISE_POINTCOUNT-1)] ^
                                       Noise->PermX[(k+dk)&(NOISE_POINTCOUNT-1)]];
      }}}
  f64 Result = TrilinearInterp(c, u,v,w);
  return Result;
}
f64 NoiseGetValueTurbulance(noise *Noise, v3f64 Pos, u32 Depth)
{
  f64   Accum   = 0.0;
  v3f64 TempPos = Pos;
  f64   Weight  = 1.0;
  for(u32 i=0;i<Depth;i++)
  {
    Accum   += Weight*NoiseGetValueSmooth(Noise, TempPos);
    Weight  *= 0.5;
    TempPos  = Scale(TempPos,2.0);
  }
  f64 Result = Abs(Accum);
  return Result;
}

#endif //RTNOISE_H
