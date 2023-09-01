#ifndef RTCAMERA_H
#define RTCAMERA_H

typedef struct camera camera;
struct camera
{
  v3f64 Origin;
  v3f64 LowerLeftCorner;
  v3f64 Horizontal;
  v3f64 Vertical;
  v3f64 u, v, w;
  f64 LenseRadius;
  f64 Time0;
  f64 Time1;
};
camera CameraInit(v3f64 LookFrom, v3f64 LookAt, v3f64 RelUp,
                  f64 VertFov, f64 AspectRatio, f64 Aperture, f64 FocusDist,
                  f64 Time0, f64 Time1)
{
  camera Result = {0};
  f64 Theta          = DegToRad64(VertFov);
  f64 Height         = Tan(Theta/2.0);
  f64 ViewportHeight = 2.0*Height;
  f64 ViewportWidth  = AspectRatio*ViewportHeight;
  
  Result.w = Normalize(Sub(LookFrom, LookAt));
  Result.u = Normalize(Cross(RelUp, Result.w));
  Result.v = Cross(Result.w, Result.u);
  
  Result.Origin     = LookFrom;
  Result.Horizontal = Scale(Result.u, ViewportWidth*FocusDist);
  Result.Vertical   = Scale(Result.v, ViewportHeight*FocusDist);
  Result.LowerLeftCorner = Sub(Result.Origin, Add(Add(Scale(Result.Horizontal, 0.5),
                                                      Scale(Result.Vertical, 0.5)),
                                                  Scale(Result.w, FocusDist)));
  Result.LenseRadius = Aperture/2.0;
  Result.Time0 = Time0;
  Result.Time1 = Time1;
  return Result;
}
ray CameraGetRay(camera *Camera, f64 s, f64 t)
{
  v3f64     rd = Scale(RandInUnitDisk(), Camera->LenseRadius);
  v3f64 Offset = Add(Scale(Camera->u, rd.x), Scale(Camera->v, rd.y));
  v3f64 Origin = Add(Camera->Origin, Offset);
  v3f64 ImagePlaneCoord = Add(Scale(Camera->Horizontal, s), Scale(Camera->Vertical  , t));
  v3f64 Dir             = Sub(Add(Camera->LowerLeftCorner, ImagePlaneCoord), Add(Camera->Origin, Offset));
  
  ray Result = RayInit(Origin, Dir,RandF64Range(Camera->Time0, Camera->Time1));
  return Result;
}
#endif //RTCAMERA_H
