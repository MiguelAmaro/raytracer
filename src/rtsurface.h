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
  SurfaceKind_BVHNode,
};
typedef enum material_kind material_kind;
enum material_kind
{
  MaterialKind_Lambert,
  MaterialKind_Metal,
  MaterialKind_Dielectric,
  MaterialKind_DiffuseLight,
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
    transformed_inst TransformedInst;
    //special
    bvh_node BvhNode;
  };
};
typedef struct material material;
struct material
{
  material_kind Kind;
  u32 TexId;
  f64 Fuzz;
  f64 IndexOfRefraction;
  //1v3f64 EmmitColor;
};
typedef struct hit hit;
struct hit
{
  v3f64 Pos;
  v3f64 Normal;
  f64  t;
  f64  u;
  f64  v;
  u32  MatId;
  b32  IsFrontFace;
};
b32   MaterialScatter(material *Material, texture *Texture, ray Ray, hit Hit, v3f64 *Atten, ray *Scattered);
v3f64 MaterialEmmited(material *Material, texture *Texture, f64 u, f64 v, v3f64 Pos);
v3f64 SphereMovingGetPos(sphere_moving *SphereMoving, f64 Time);
b32   SurfaceListHit(surface *Surfaces, u32 SurfaceCount, hit *Hit, ray Ray, f64 Mint, f64 Maxt);
b32   SurfaceHit(surface *Surface, hit *Hit, ray Ray, f64 Mint, f64 Maxt);
#endif //RTSURFACE_H
