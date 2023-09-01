#ifndef RTBVH_H
#define RTBVH_H

#define AABB_ISBOUNDABLE (1)
#define AABB_ISNOTBOUNDABLE (0)

typedef r3f64 aabb;
b32 AABBInitSurface(surface *Surface, f64 Time0, f64 Time1, aabb *Output);

aabb AABBInit(v3f64 min, v3f64 max)
{
  aabb Result = { .min = min, .max = max };
  return Result;
}
aabb AABBInitContainer(aabb a, aabb b)
{
  v3f64 min = V3f64(fmin(a.min.x, b.min.x),
                    fmin(a.min.y, b.min.y),
                    fmin(a.min.z, b.min.z));
  v3f64 max = V3f64(fmax(a.max.x, b.max.x),
                    fmax(a.max.y, b.max.y),
                    fmax(a.max.z, b.max.z));
  aabb Result = AABBInit(min, max);
  return Result;
}
b32 AABBInitBVHNode(bvh_node *BvhNode, aabb *Output)
{
  WriteToRef(Output, BvhNode->AABB);
  return AABB_ISBOUNDABLE;
}
b32 AABBInitRectXY(rect *Rect, aabb *Output)
{
  r3f64 p = Rect->Points;
  aabb Result = AABBInit(V3f64(p.x0,p.y0,Rect->Offset-0.0001), V3f64(p.x1,p.y1,Rect->Offset+0.0001));
  WriteToRef(Output, Result);
  return AABB_ISBOUNDABLE;
}
b32 AABBInitRectXZ(rect *Rect, aabb *Output)
{
  r3f64 p = Rect->Points;
  aabb Result = AABBInit(V3f64(p.x0,Rect->Offset-0.0001,p.z0), V3f64(p.x1,Rect->Offset+0.0001,p.z1));
  WriteToRef(Output, Result);
  return AABB_ISBOUNDABLE;
}
b32 AABBInitRectYZ(rect *Rect, aabb *Output)
{
  r3f64 p = Rect->Points;
  aabb Result = AABBInit(V3f64(Rect->Offset-0.0001,p.y0,p.z0), V3f64(Rect->Offset+0.0001,p.y1,p.z1));
  WriteToRef(Output, Result);
  return AABB_ISBOUNDABLE;
}
b32 AABBInitSphere(sphere *Sphere, aabb *Output)
{
  aabb Result = AABBInit(Sub(Sphere->Pos, V3f64(Sphere->Radius, Sphere->Radius, Sphere->Radius)),
                         Add(Sphere->Pos, V3f64(Sphere->Radius, Sphere->Radius, Sphere->Radius)));
  WriteToRef(Output, Result);
  return AABB_ISBOUNDABLE;
}
b32 AABBInitSphereMoving(sphere_moving *Sphere, f64 Time0, f64 Time1, aabb *Output)
{
  aabb Result0 = AABBInit(Sub(SphereMovingGetPos(Sphere, Time0), V3f64(Sphere->Radius, Sphere->Radius, Sphere->Radius)),
                          Add(SphereMovingGetPos(Sphere, Time0), V3f64(Sphere->Radius, Sphere->Radius, Sphere->Radius)));
  aabb Result1 = AABBInit(Sub(SphereMovingGetPos(Sphere, Time1), V3f64(Sphere->Radius, Sphere->Radius, Sphere->Radius)),
                          Add(SphereMovingGetPos(Sphere, Time1), V3f64(Sphere->Radius, Sphere->Radius, Sphere->Radius)));
  WriteToRef(Output, AABBInitContainer(Result0, Result1));
  return AABB_ISBOUNDABLE;
}
b32 AABBInitBox(box *Box, aabb *Output)
{
  aabb Result = { .min = Box->min, .max = Box->max };
  WriteToRef(Output, Result);
  return AABB_ISBOUNDABLE;
}
b32 AABBInitTransformedInst(transformed_inst *Instance, aabb *Output)
{
  aabb Result = {0};
  // TODO(MIGUEL): Clean up and condense code
  if(Instance->Kind == (TransformKind_Translate | TransformKind_RotateY))
  {
    if(!AABBInitSurface(Instance->Instance, 0,0, Output)) return AABB_ISNOTBOUNDABLE;
    Result = Instance->AABB;
    WriteToRef(Output, Result);
    Result = AABBInit(Add(Output->min, Instance->Offset),
                      Add(Output->max, Instance->Offset));
    WriteToRef(Output, Result);
  }
  if(Instance->Kind == TransformKind_Translate)
  {
    if(!AABBInitSurface(Instance->Instance, 0,0, Output)) return AABB_ISNOTBOUNDABLE;
    Result = AABBInit(Add(Output->min, Instance->Offset),
                      Add(Output->max, Instance->Offset));
    WriteToRef(Output, Result);
  }
  if(Instance->Kind == TransformKind_RotateY)
  {
    Result = Instance->AABB;
    WriteToRef(Output, Result);
  }
  return AABB_ISBOUNDABLE;
}
b32 AABBInitConstantMedium(constant_medium *Medium, aabb *Output)
{
  b32 Result = AABBInitSurface(Medium->Boundary, 0.0,0.0, Output);
  return Result;
}
b32 AABBInitFlipFace(flip_face *FlipFace, aabb *Output)
{
  b32 Result = AABBInitSurface(FlipFace->Surface, 0.0,0.0, Output);
  return Result;
}
b32 AABBInitSurface(surface *Surface, f64 Time0, f64 Time1, aabb *Output)
{
  b32 Result = AABB_ISNOTBOUNDABLE;
  switch(Surface->Kind)
  {
    case SurfaceKind_Sphere: {
      Result = AABBInitSphere(&Surface->Sphere, Output);
    }break;
    case SurfaceKind_SphereMoving: {
      Result = AABBInitSphereMoving(&Surface->SphereMoving, Time0, Time1, Output);
    }break;
    case SurfaceKind_RectXY: {
      Result = AABBInitRectXY(&Surface->Rect, Output);
    }break;
    case SurfaceKind_RectXZ: {
      Result = AABBInitRectXZ(&Surface->Rect, Output);
    }break;
    case SurfaceKind_RectYZ: {
      Result = AABBInitRectYZ(&Surface->Rect, Output);
    }break;
    case SurfaceKind_Box: {
      Result = AABBInitBox(&Surface->Box, Output);
    }break;
    case SurfaceKind_ConstantMedium: {
      Result = AABBInitConstantMedium(&Surface->ConstantMedium, Output);
    }break;
    case SurfaceKind_TransformedInst: {
      Result = AABBInitTransformedInst(&Surface->TransformedInst, Output);
    }break;
    case SurfaceKind_BVHNode: {
      Result = AABBInitBVHNode(&Surface->BvhNode, Output);
    }break;
    case SurfaceKind_FlipFace: {
      Result = AABBInitFlipFace(&Surface->FlipFace, Output);
    }break;
  }
  return Result;
}
// NOTE(MIGUEL): In the tut the bvh is expressed in terms of a hierarchical class called hittable
//               one of the sub classes of hittable is the bvh node and it is the only one that has
//               left and right links but the pointers are pointers to hittable objects.
//               This is like having a node type with a union of all the surfaces that i can do 
//               intersection tests on like (spheres, moving spheres, planes, bvhnode). the bvh node
//               has a parametric obj(?) and links.
//               Using this approach would entail having a single list of this one type and just having push function
//               for a given surface fill in and write to the appropriate union type.
//
typedef int aabb_comparator(const void *a, const void *b);
int AabbComp(const void *a, const void *b, u32 axis)
{
  const surface *sa = a; const surface *sb = b;
  aabb Aabb_a = {0};
  aabb Aabb_b = {0};
  if(!AABBInitSurface(sa, 0, 0, &Aabb_a) ||
     !AABBInitSurface(sb, 0, 0, &Aabb_b))
  {
    fprintf(stderr, "error no bounding box bvh node");
  }
  return Aabb_a.min.e[axis]<Aabb_b.min.e[axis];
}
int AabbXComp(const void *a, const void *b) { return AabbComp(a,b,0); }
int AabbYComp(const void *a, const void *b) { return AabbComp(a,b,1); }
int AabbZComp(const void *a, const void *b) { return AabbComp(a,b,2); }

void BVHInit(surface *BvhSurface, surface *SurfaceList, u64 Start, u64 End, f64 Time0, f64 Time1, arena *Arena)
{
  BvhSurface->Kind = SurfaceKind_BVHNode;
  bvh_node *BvhNode = &BvhSurface->BvhNode; 
  surface *List = SurfaceList;
  u32 Axis = RandInt(0, 3);
  aabb_comparator *Comparator = ((Axis == 0)?AabbXComp:
                                 (Axis == 1)?AabbYComp: AabbZComp);
  u64 Span = End-Start;
  
  // assigning surface from list to a leaf (look at the span conditions)
  if(Span == 1)
  {
    BvhNode->Left = BvhNode->Right = &List[Start];
  }
  else if(Span == 2)
  {
    if(Comparator(&List[Start], &List[Start + 1]))
    {
      BvhNode->Left  = &List[Start];
      BvhNode->Right = &List[Start+1];
    }
    else
    {
      BvhNode->Left  = &List[Start+1];
      BvhNode->Right = &List[Start];
    }
  } else {
    // in this case the span is greater than the link count we know we are not
    // a leaf and need to allocate a link
    qsort(&List[Start], Span, sizeof(surface), Comparator);
    u64 Mid = Start + Span/2;
    
    BvhNode->Left  = ArenaPushType(Arena, surface);
    BvhNode->Right = ArenaPushType(Arena, surface);
    BVHInit(BvhNode->Left , List, Start, Mid, Time0, Time1, Arena);
    BVHInit(BvhNode->Right, List, Mid  , End, Time0, Time1, Arena);
  }
  aabb BoxLeft  = {0};
  aabb BoxRight = {0};
  if(!AABBInitSurface(BvhNode->Left , Time0, Time1, &BoxLeft) ||
     !AABBInitSurface(BvhNode->Right, Time0, Time1, &BoxRight))
  {
    fprintf(stderr, "some message, should check tut");
  }
  BvhNode->AABB = AABBInitContainer(BoxLeft, BoxRight);
  return;
}


#endif //RTBVH_H
