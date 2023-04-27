#ifndef RTSURFACE_H
#define RTSURFACE_H

typedef enum surface_kind surface_kind;
enum surface_kind
{
  SurfaceKind_Sphere,
  SurfaceKind_SphereMoving,
  SurfaceKind_Plane,
  SurfaceKind_BVHNode,
};
typedef enum material_kind material_kind;
enum material_kind
{
  MaterialKind_Lambert,
  MaterialKind_Metal,
  MaterialKind_Dielectric,
};
typedef struct plane plane;
struct plane
{
  v3f64 Normal;
  f64   Dist;
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
typedef struct surface surface;
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
    sphere Sphere;
    plane  Plane;
    sphere_moving SphereMoving;
    //special
    bvh_node BvhNode;
  };
};
typedef struct material material;
struct material
{
  material_kind Kind;
  u32 TexId;
  //v3f64 Albedo; 
  f64   Fuzz;
  f64   IndexOfRefraction;
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
v3f64 SphereMovingGetPos(sphere_moving *SphereMoving, f64 Time);
#endif //RTSURFACE_H