#ifndef RTINTEGRATOR_H
#define RTINTEGRATOR_H

typedef enum pdf_kind pdf_kind;
enum pdf_kind
{
  PdfKind_Cosine,
  PdfKind_Surface,
};
typedef struct pdf pdf;
struct pdf
{
  pdf_kind Kind;
  m3f64    Basis; //is ortho normal ofcourse
};

// ORTHONORMAL BASIS
#define OrthoNormBasisGetLocal(...) _Generic(ARG2(__VA_ARGS__), f64:   OrthoNormBasisGetLocal_3f64, v3f64: OrthoNormBasisGetLocal_v3f64)(__VA_ARGS__)
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
// END ORTHONORMAL BASIS


//UNIDIRECTIONAL LIGHT
typedef struct flip_face flip_face;
struct flip_face
{
  surface *Surface;
};
b32 SurfaceHitFlipFace(void)
{
  Assert(!"Not Implemented");
  return 0;
}
b32 AABBInitHitFlipFace(void)
{
  Assert(!"Not Implemented");
  return 0;
}
//END UNIDIRECTIONAL LIGHT


void PdfInit(pdf_kind Kind)
{
  return;
}
///MOVE TO UTILS

///MOVE TO UTILS
f64 PdfCosinePdf(pdf *Pdf, v3f64 w)
{
  //listing 24
  f64 Cosine = Dot(Normalize(w), Pdf->Basis.w);
  f64 Result = (Cosine<0.0)?0.0:Cosine/Pi64;
  return Result;
}
f64 PdfGetValue(pdf *Pdf, v3f64 w)
{
  //listing 24
  f64 Result = PdfCosinePdf(Pdf, w);
  return Result;
}
v3f64 PdfGenerate(pdf *Pdf, v3f64 w)
{
  //listing 24
  v3f64 Result = OrthoNormBasisGetLocal(Pdf->Basis, w);
  return Result;
}

//TODO : listing 16 DONE!!!!
//TODO : listing 17 DONE!!!!
//TODO : listing 18 DONE!!!!
//TODO : listing 19
//TODO : listing 20
//TODO : listing 25


#endif //RTINTEGRATOR_H
