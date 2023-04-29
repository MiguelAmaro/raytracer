#include <stdio.h>
#include <stdint.h>
#include <math.h>

// headers
#include "rttypes.h"
#include "rtatomics.h"
#include "rtos.h"
#include "rtmath.h"
#include "rtmemory.h"
#include "rtutil.h"
#include "rtray.h"
#include "rtnoise.h"
#include "rtcamera.h"
#include "rttexture.h"
#include "rtsurface.h"
#include "rtworld.h"
#include "rtbvh.h"
#include "rtwork.h"
// source
#include "rtmemory.c"
#include "rtray.c"
#include "rtsurface.c"
#include "rtworld.c"
#include "rtprofile.h"

#include "rtscenes.h"
b32 RenderTile(work_queue *Queue)
{
  u64 WorkOrderIndex = LockedAddAndGetLastValue(&Queue->NextWorkOrderIndex, 1);
  if(WorkOrderIndex >= Queue->WorkOrderCount) { return 0;}
  work_order *Order = &Queue->WorkOrders[WorkOrderIndex];
  fprintf(stderr, "\n new thread work started for work id(%lld)...", WorkOrderIndex);
  
  /// destructure
  world        *World = Order->World;
  camera      *Camera = Order->Camera;
  u32 SamplesPerPixel = Order->SamplesPerPixel;
  u32 MaxDepth        = Order->MaxDepth;
  
  u8 *ImageBuffer  = Order->ImageBuffer;
  u32 ImageWidth   = Order->ImageWidth;
  u32 ImageHeight  = Order->ImageHeight;
  s32 MinX         = Order->MinX;
  s32 MinY         = Order->MinY;
  s32 OnePastLastX = Order->OnePastLastX;
  s32 OnePastLastY = Order->OnePastLastY;
  
  for(s32 y = OnePastLastY-1; y>=MinY; --y)
  {
    for(s32 x = MinX; x<OnePastLastX; ++x)
    {
      v3f64 PixelColor = V3f64(0.0,0.0,0.0);
      for(u32 s = 0; s<SamplesPerPixel; s++)
      {
        // NOTE(MIGUEL): The anti-aliasing already requires an average. The operation can be
        //               used for motion blur since we can randomly choose a sphere position
        //               in the line segment it moves in for each sample we take for the anti-aliasing.
        // TODO(MIGUEL): Read the random number generator section
        f64 u = (f64)(x + RandF64Uni())/(f64)(ImageWidth-1);
        f64 v = (f64)(y + RandF64Uni())/(f64)(ImageHeight-1);
        ray Ray = CameraGetRay(Camera, u, v);
        PixelColor = Add(PixelColor, RayColor(Ray, MaxDepth, World));
      }
      WriteColor(ImageGetPixel(ImageBuffer, ImageWidth, x,y), PixelColor, SamplesPerPixel);
    }
  }
  LockedAddAndGetLastValue(&Queue->TileRetiredCount, 1);
  return 1;
}
DWORD WorkProcRenderTiles(void *Param)
{
  work_queue *Queue = (work_queue *)Param;
  while(RenderTile(Queue)) {};
  return 0;
}
int main(void)
{
  u8 wd[256] = {0};
  fprintf(stderr, "hello raytracer [%s]\n", OSGetWorkingDir(wd, 256));
  OSEntropyInit();
  RandSetSeed();
  
  // IMAGE
  f64 AspectRatio = 16.0/9.0;
  s32 ImageWidth  = 800;
  s32 ImageHeight = (int)(ImageWidth/AspectRatio);
  s32 SamplesPerPixel = 50;
  s32 MaxDepth = 10;
  
  // WORLD
  world World = {0};
  WorldInit(&World);
  
  // SCENE
  camera Camera = {0};
  //SceneRandom(&World, &Camera, AspectRatio);
  //SceneBVHTest(&World, &Camera, AspectRatio);
  //SceneTwoSpheres(&World, &Camera, AspectRatio);
  //SceneTwoPerlinSpheres(&World, &Camera, AspectRatio);
  SceneEarthSolo(&World, &Camera, AspectRatio);
  
  
  // WORK
  u32 CoreCount = OSGetCoreCount();
  u8 *ImageBuffer = OSMemoryAlloc(sizeof(u32)*ImageWidth*ImageHeight);
  work_queue WorkQueue = WorkQueueInit(TileFmt_uint_R8G8B8A8,
                                       ImageBuffer, ImageWidth, ImageHeight,
                                       &World, MaxDepth,
                                       &Camera, SamplesPerPixel, CoreCount);
  u64 Begin = OSTimerGetTick();
  u64 ThreadHandles[64] = {0};
  for(u32 CoreIdx=0; CoreIdx<CoreCount; CoreIdx++)
  {
    ThreadHandles[CoreIdx] = OSThreadCreate(&WorkQueue, WorkProcRenderTiles);
  }
  while(RenderTile(&WorkQueue)) {
    fprintf(stderr,"\ntiles %lld of %d completed\n", WorkQueue.TileRetiredCount, WorkQueue.TileTotalCount);
  };
  OSThreadSync(ThreadHandles, CoreCount);
  u64 End = OSTimerGetTick();
  f64 SecondsElapsed = OSTimerGetSecondsElepsed(Begin, End);
  fprintf(stderr,"done..\nsec elapsed: %f\nmin elapsed: %f\n", SecondsElapsed, SecondsElapsed/60.0);
  
  
  // RENDER AS PPM
  fprintf(stderr, "\n\n writing as ppm..\n");
  fprintf(stdout, "P3\n%d %d\n255\n", ImageWidth, ImageHeight);
  for(s32 y = ImageHeight-1; y>=0; --y)
  {
    fprintf(stderr, "\rscan lines remaining... %d ", y);
    fflush(stderr);
    for(s32 x = 0; x<ImageWidth; ++x)
    {
      u32 *Pixel = ImageGetPixel(ImageBuffer, ImageWidth, x,y);
      WriteToPPM(*Pixel);
    }
    fprintf(stdout,"#newline\n");
  }
  fprintf(stderr, "done..\n");
  
  OSEntropyRelease();
  return 0;
}