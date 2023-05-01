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
  fprintf(stderr, "new tile[%5lld] started by thread[%5lu]...\n", WorkOrderIndex, GetCurrentThreadId());
  
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
        f64 u = ((f64)x + RandF64Bi())/(f64)(ImageWidth-1);
        f64 v = ((f64)y + RandF64Bi())/(f64)(ImageHeight-1);
        ray Ray = CameraGetRay(Camera, u, v);
        PixelColor = Add(PixelColor, RayColor(Ray, MaxDepth, World));
      }
      WriteColor(ImageGetPixel(ImageBuffer, ImageWidth, x,y), PixelColor, SamplesPerPixel);
    }
  }
  LockedAddAndGetLastValue(&Queue->TileRetiredCount, 1);
  return 1;
}
int main(void)
{
  u8 wd[256] = {0};
  fprintf(stderr, "hello raytracer [%s]\n", OSGetWorkingDir(wd, 256));
  OSEntropyInit();
  RandSetSeed();
#if 0
  int N = 1000;
  int InsideCircle = 0;
  int runs = 0;
  while(1)
  {
    runs++;
    f64 x = RandF64Bi();
    f64 y = RandF64Bi();
    if(x*x + y*y < 1) InsideCircle++;
    if(runs%100000 == 0) 
    {
      printf("\r                                                       ");
      printf("\rEstmate of Pi = %f", 4.0*((f64)InsideCircle)/(f64)runs);
    }
  }
  return;
  //End
#endif
  f64 AspectRatio = 1.0;//16.0/9.0;
  s32 ImageWidth  = 400;
  s32 ImageHeight = (int)(ImageWidth/AspectRatio);
  s32 SamplesPerPixel = 200;
  s32 MaxDepth = 50;
  
  // WORLD
  world *World = WorldInit(V3f64(0.0,0.0,0.0));
  
  // SCENE
  camera Camera = {0};
  //SceneRandom(&World, &Camera, AspectRatio);
  //SceneBVHTest(&World, &Camera, AspectRatio);
  //SceneTwoSpheres(&World, &Camera, AspectRatio); 
  //SceneTwoPerlinSpheres(&World, &Camera, AspectRatio);
  //SceneEarthSolo(&World, &Camera, AspectRatio);
  //SceneSimpleLight(&World, &Camera, AspectRatio);
  SceneCornellBox(World, &Camera, AspectRatio);
  //SceneTestCornellBox(World, &Camera, AspectRatio);
  
  // WORK
  u8 *ImageBuffer = OSMemoryAlloc(sizeof(u32)*ImageWidth*ImageHeight);
  u32 CoreCount   = OSGetCoreCount(); //TODO: deduce the actual core count.
  work_queue WorkQueue = WorkQueueInit(TileFmt_uint_R8G8B8A8,
                                       ImageBuffer, ImageWidth, ImageHeight,
                                       World, MaxDepth,
                                       &Camera, SamplesPerPixel, CoreCount);
  fprintf(stderr,
          "starting raytrace...\n"
          "    trace information:\n"
          "    physical core count: %d\n"
          "    resolution         : %dx%d\n"
          "    samples per pixel  : %d\n"
          "    bounce max depth   : %d\n"
          "    tile count         : %d\n"
          "    tile dim           : %dx%d\n",
          CoreCount,
          ImageWidth, ImageHeight,
          SamplesPerPixel,
          MaxDepth,
          WorkQueue.TileTotalCount,
          WorkQueue.TileWidth, WorkQueue.TileHeight);
  
  u64 Begin = OSTimerGetTick();
  WorkQueueLaunchThreads(&WorkQueue);
  WorkQueueBeginWork(&WorkQueue);
  while(RenderTile(&WorkQueue))
  {
    fprintf(stderr,"\ntiles: %lld of %d completed\n", WorkQueue.TileRetiredCount, WorkQueue.TileTotalCount);
  };
  OSThreadSync(WorkQueue.ThreadHandles, CoreCount);
  u64 End = OSTimerGetTick();
  f64 SecondsElapsed = OSTimerGetSecondsElepsed(Begin, End);
  fprintf(stderr,"raytrace finished..\nsec elapsed: %f\nmin elapsed: %f\n", SecondsElapsed, SecondsElapsed/60.0);
  
  
  // RENDER AS PPM
  ExportAsPPM("F:\\Dev\\raytracer\\data\\image.ppm", ImageBuffer, ImageWidth, ImageHeight);
  ExportAsPNG("F:\\Dev\\raytracer\\data\\image.png", ImageBuffer, ImageWidth, ImageHeight);
  
  OSEntropyRelease();
  fprintf(stderr,"done!\n");
  return 0;
}