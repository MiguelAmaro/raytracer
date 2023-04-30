#ifndef RTMATH_H
#define RTMATH_H

#define Min(a, b) ((a<b)?a:b)
#define Max(a, b) ((a>b)?a:b)
#define Clamp(x, Low, High) (Min(Max(Low, x), High))
#define Pi64  (1.0*3.1415926535897932385)
#define Tau64 (2.0*3.1415926535897932385)

#define ARG1(a, ...) (a)

#define Mul(a, b)     _Generic((a), v3f64: Mul_v3f64)((a), (b))
#define Dot(a, b)     _Generic((a), v3f64: Dot_v3f64)((a), (b))
#define Cross(a, b)   _Generic((a), v3f64: Cross_v3f64)((a), (b))
#define Add(a, b)     _Generic((a), v3f64: Add_v3f64)((a), (b))
#define Sub(a, b)     _Generic((a), v3f64: Sub_v3f64)((a), (b))
#define Scale(a, t)   _Generic((a), v3f64: Scale_v3f64)((a), (t))
#define Power(a, p)   _Generic((a), f64: Power_f64, v3f64: Power_v3f64)((a), (p))
#define Reflect(a, n) _Generic((a), v3f64: Reflect_v3f64)((a), (n))
#define Lerp(a, b, t) _Generic((a), v3f64: Lerp_v3f64)((a), (b), (t))
#define Tan(a)        _Generic((a), f64: Tan_f64)((a))
#define Cos(a)        _Generic((a), f64: Cos_f64)((a))
#define Sin(a)        _Generic((a), f64: Sin_f64)((a))
#define Floor(a)        _Generic((a), f64: Floor_f64)((a))
#define Ceil(a)        _Generic((a), f64: Ceil_f64)((a))
#define Abs(a)        _Generic((a), f64: Abs_f64)((a))
#define Length(a)     _Generic((a), v3f64: Length_v3f64)((a))
#define LengthSquared(a) _Generic((a), v3f64: LengthSquared_v3f64)((a))
#define SquareRoot(a)    _Generic((a), f64: SquareRoot_f64, v3f64: SquareRoot_v3f64)((a))
#define Normalize(a)     _Generic((a), v3f64: Normalize_v3f64)((a))


// RANDOM
#include "pcg64.h"
pcg64 GlobalRNG = {0};
void RandSetSeed(void)
{
  u64 ResultA = 0;
  u64 ResultB = 0;
  OSEntropyGen(&ResultA, sizeof(u64));
  OSEntropyGen(&ResultB, sizeof(u64));
  pcg64_seed(&GlobalRNG, ResultA, ResultB);
  return;
}
u64 RandRange(u64 Low, u64 High)
{
  u64 r = pcg64_range(&GlobalRNG, Low, High);
  return r;
}
f64 RandF64Uni(void)
{
  f64 r = pcg64_nextd(&GlobalRNG);
  return r;
}
f64 RandF64Range(f64 low, f64 high)
{
  Assert(low<=high);
  f64 d = high-low;
  f64 t = RandF64Uni();
  f64 r = low + d*t;
  return r;
}
f64 RandF64Bi(void)
{
  f64 r = RandF64Range(-1.0,1.0);
  return r;
}
// SCALAR
f64 SquareRoot_f64(f64 a)
{
  f64 r = sqrt(a);
  return r;
}
f64 Power_f64(f64 a, f64 p)
{
  f64 r = pow(a, p);
  return r;
}
f64 Tan_f64(f64 a)
{
  f64 r = tan(a);
  return r;
}
f64 Cos_f64(f64 a)
{
  f64 r = cos(a);
  return r;
}
f64 Sin_f64(f64 a)
{
  f64 r = sin(a);
  return r;
}
f64 Floor_f64(f64 a)
{
  f64 r = floor(a);
  return r;
}
f64 Ceil_f64(f64 a)
{
  f64 r = ceil(a);
  return r;
}
f64 Abs_f64(f64 a)
{
  f64 r = fabs(a);
  return r;
}

// VECTOR
typedef union v3f64 v3f64;
union v3f64
{
  struct { f64 x; f64 y; f64 z; };
  struct { f64 r; f64 g; f64 b; };
  f64 e[3];
};
#define V3f64(...) _Generic(ARG1(__VA_ARGS__), f64: V3f64_3f64, s32: V3f64_3s32)(__VA_ARGS__)

v3f64 V3f64_3f64(f64 x, f64 y, f64 z)
{
  v3f64 r = {x,y,z};
  return r;
}
v3f64 V3f64_3s32(s32 x, s32 y, s32 z) { return V3f64_3f64((f64)x,(f64)y,(f64)z); }
v3f64 V3f64RandBi(void)
{
  v3f64 r = {RandF64Bi(),RandF64Bi(),RandF64Bi()};
  return r;
}
v3f64 V3f64RandUni(void)
{
  v3f64 r = {RandF64Uni(),RandF64Uni(),RandF64Uni()};
  return r;
}
v3f64 Add_v3f64(v3f64 a, v3f64 b)
{
  v3f64 r = {a.x+b.x, a.y+b.y, a.z+b.z};
  return r;
}
v3f64 Sub_v3f64(v3f64 a, v3f64 b)
{
  v3f64 r = {a.x-b.x, a.y-b.y, a.z-b.z};
  return r;
}
v3f64 Mul_v3f64(v3f64 a, v3f64 b)
{
  v3f64 r = {a.x*b.x, a.y*b.y, a.z*b.z};
  return r;
}
f64 Dot_v3f64(v3f64 a, v3f64 b)
{
  f64 r = a.x*b.x + a.y*b.y + a.z*b.z;
  return r;
}
v3f64 Cross_v3f64(v3f64 a, v3f64 b)
{
  v3f64 r = V3f64(a.y*b.z - a.z*b.y,
                  a.z*b.x - a.x*b.z,
                  a.x*b.y - a.y*b.x);
  return r;
}
v3f64 Scale_v3f64(v3f64 a, f64 t)
{
  v3f64 r = {a.x*t, a.y*t, a.z*t};
  return r;
}
v3f64 Power_v3f64(v3f64 a, f64 p)
{
  v3f64 r = {Power(a.x, p), Power(a.y, p), Power(a.z, p)};
  return r;
}
v3f64 Lerp_v3f64(v3f64 a, v3f64 b, f64 t)
{
  v3f64 r = Add(Scale(a, 1.0-t), Scale(b, t));
  return r;
}
f64 LengthSquared_v3f64(v3f64 a)
{
  f64 r = a.x*a.x + a.y*a.y + a.z*a.z;
  return r;
}
f64 Length_v3f64(v3f64 a)
{
  f64 r = sqrt(LengthSquared(a));
  return r;
}
v3f64 SquareRoot_v3f64(v3f64 a)
{
  v3f64 r = {SquareRoot(a.x),SquareRoot(a.y),SquareRoot(a.z)};
  return r;
}
v3f64 Reflect_v3f64(v3f64 a, v3f64 n)
{
  v3f64 r = Sub(a, Scale(n, 2.0*Dot(a, n)));
  return r;
}
v3f64 Normalize_v3f64(v3f64 a)
{
  v3f64 r = Scale(a, 1.0/Length(a));
  return r;
}

// TRIG
inline f64 DegToRad64(f64 Degrees)
{
  f64 r = Degrees*Pi64/180.0;
  return r;
}

typedef union r3f64 r3f64;
union r3f64
{
  struct { v3f64 min; v3f64 max; };
  struct { f64 x0; f64 y0; f64 z0; f64 x1; f64 y1; f64 z1; };
  f64 e[6];
};
#define R3f64(...) _Generic(ARG1(__VA_ARGS__), f64: R3f64_6f64)(__VA_ARGS__)
r3f64 R3f64_6f64(f64 x0, f64 y0, f64 z0, f64 x1, f64 y1, f64 z1)
{
  r3f64 r = { 
    .x0 = x0, .x1 = x1,
    .y0 = y0, .y1 = y1,
    .z0 = z0, .z1 = z1
  };
  return r;
}

#endif //RTMATH_H
