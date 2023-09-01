#ifndef RTSURFACE_H
#define RTSURFACE_H

#define HIT_DETECTED (1)
#define HIT_NOTDETECTED (0)
#define SCATTER_PROCESS (1)
#define SCATTER_IGNORE  (0)
typedef enum surface_kind surface_kind;
enum surface_kind
{
  SurfaceKind_Sphere,
  SurfaceKind_SphereMoving,
  //SurfaceKind_Plane,
  SurfaceKind_RectXY,
  SurfaceKind_RectXZ,
  SurfaceKind_RectYZ,
  SurfaceKind_Box,
  SurfaceKind_TransformedInst,
  SurfaceKind_FlipFace,
  SurfaceKind_ConstantMedium,
  SurfaceKind_BVHNode,
};
typedef enum material_kind material_kind;
enum material_kind
{
  MaterialKind_Lambert,
  MaterialKind_Metal,
  MaterialKind_Dielectric,
  MaterialKind_DiffuseLight,
  MaterialKind_Isotropic,
};
typedef struct surface surface;
typedef struct plane plane;
struct plane
{
  v3f64 Normal;
  f64   Dist;
  u32   MatId;
};
typedef struct rect rect;
struct rect
{
  r3f64 Points;
  f64   Offset;
  u32   MatId;
};
typedef struct sphere sphere;
struct sphere
{
  v3f64 Pos;
  f64   Radius;
  u32   MatId;
};
typedef struct sphere_moving sphere_moving;
struct sphere_moving
{
  v3f64 Pos0;
  v3f64 Pos1;
  f64   Time0;
  f64   Time1;
  f64   Radius;
  u32   MatId;
};
typedef struct box box;
struct box
{
  v3f64 min;
  v3f64 max;
  u32 MatId;
  surface *SidesList;
};
typedef struct flip_face flip_face;
struct flip_face
{
  surface *Surface;
};
typedef enum transform_kind transform_kind;
enum transform_kind
{
  TransformKind_Translate = (1<<0),
  TransformKind_RotateY   = (1<<1),
};
typedef struct transformed_inst transformed_inst;
struct transformed_inst
{
  transform_kind Kind;
  surface *Instance;
  // translation
  v3f64 Offset;
  // y rotation
  f64 CosTheta;
  f64 SinTheta;
  b32 HasBox;
  r3f64 AABB; 
};
typedef struct constant_medium constant_medium;
struct constant_medium
{
  surface *Boundary;
  u32 MatId;
  f64 NegInvDensity;
};
typedef struct bvh_node bvh_node;
struct bvh_node
{
  surface *Left;
  surface *Right;
  r3f64 AABB; 
};
struct surface
{
  surface_kind Kind;
  union {
    sphere        Sphere;
    plane         Plane;
    sphere_moving SphereMoving;
    rect          Rect;
    box           Box;
    flip_face     FlipFace;
    transformed_inst TransformedInst;
    constant_medium ConstantMedium;
    //special
    bvh_node BvhNode;
  };
};
typedef struct material material;
struct material
{
  material_kind Kind;
  u32 TexId; //isotropic
  f64 Fuzz;
  f64 IndexOfRefraction;
  //1v3f64 EmmitColor;
};
typedef struct hit_info hit_info;
struct hit_info
{
  v3f64 Pos;
  v3f64 Normal;
  f64  t;
  f64  u;
  f64  v;
  u32  MatId;
  b32  IsFrontFace;
};
typedef enum pdf_kind pdf_kind;
enum pdf_kind
{
  PdfKind_Cosine,
  PdfKind_Mixture,
  PdfKind_Surface,
};
typedef struct pdf pdf;
struct pdf
{
  pdf_kind Kind;
  m3f64    Basis; //is ortho normal ofcourse
  // surface
  v3f64    Origin;
  surface *Surface;
  // mixture
  pdf *MixPdf[2];
};
typedef struct scatter_info scatter_info;
struct scatter_info
{
  ray SpecularRay;
  b32 IsSpecular;
  v3f64 Atten;
  pdf *Pdf;
};

b32   SurfaceListHit(surface *Surfaces, u32 SurfaceCount, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt);
b32   SurfaceHit(surface *Surface, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt);
b32   SurfaceRectYZHit(rect *Rect, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt);
b32   SurfaceSphereHit(sphere *Sphere, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt);
b32   SurfaceSphereMovingHit(sphere_moving *Sphere, hit_info *Hit, ray Ray, f64 Mint, f64 Maxt);
b32   MaterialScatter(material *Material, texture *Texture, ray Ray, hit_info *Hit, scatter_info *Scattered, scratch *Scratch);
f64   MaterialScatterPdf(material *Material, hit_info Hit, ray Scattered);
v3f64 MaterialEmmited(texture *Texture, hit_info Hit);

//helpers
v3f64 SphereMovingGetPos(sphere_moving *SphereMoving, f64 Time);
v3f64 SurfaceGenRandom(surface *Surface, v3f64 Origin);
f64   SurfaceGetPdfValue(surface *Surface, v3f64 Origin, v3f64 v);
m3f64 OrthoNormBasisFromNormal(v3f64 n);
v3f64 OrthoNormBasisGetLocal_v3f64(m3f64 Basis, v3f64 a);
v3f64 OrthoNormBasisGetLocal_3f64(m3f64 Basis, f64 x, f64 y, f64 z);
m3f64 OrthoNormBasisInit(v3f64 u, v3f64 v, v3f64 w);

#define OrthoNormBasisGetLocal(...) _Generic(ARG2(__VA_ARGS__), f64:   OrthoNormBasisGetLocal_3f64, v3f64: OrthoNormBasisGetLocal_v3f64)(__VA_ARGS__)

#endif //RTSURFACE_H
