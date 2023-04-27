#ifndef RTWORK_H
#define RTWORK_H

typedef enum tile_fmt tile_fmt;
enum tile_fmt
{
  TileFmt_float_R32G32B32A32,
  TileFmt_uint_R8G8B8A8,
};
typedef struct work_order work_order;
struct work_order
{
  u32 Id;
  world   *World;
  camera *Camera;
  u32 SamplesPerPixel;
  u32 MaxDepth;
  u8 *ImageBuffer;
  u32 ImageWidth;
  u32 ImageHeight;
  u32 MinX;
  u32 MinY;
  u32 OnePastLastX;
  u32 OnePastLastY;
};
typedef struct work_queue work_queue;
struct work_queue
{
  work_order *WorkOrders;
  u32         WorkOrderCount;
  
  volatile u64 NextWorkOrderIndex;
  volatile u64 TotalBounceCount;
  volatile u64 TileRetiredCount;
  
  //TILE METADATA
  tile_fmt TileFormat;
  u32 TileWidth;
  u32 TileHeight;
  u32 TileCountX;
  u32 TileCountY;
  u32 TileTotalCount;
};

work_order WorkOrderInit(u8 *ImageBuffer, u32 ImageWidth, u32 ImageHeight,
                         world *World, u32 MaxDepth,
                         u32 MinX, u32 MinY,
                         u32 OnePastLastX, u32 OnePastLastY,
                         camera *Camera, u32 SamplesPerPixel)
{
  work_order Result = {
    .World = World,
    .Camera = Camera,
    .SamplesPerPixel = SamplesPerPixel,
    .MaxDepth = MaxDepth,
    .ImageBuffer = ImageBuffer,
    .ImageWidth = ImageWidth,
    .ImageHeight = ImageHeight,
    .MinX = MinX,
    .MinY = MinY,
    .OnePastLastX = OnePastLastX,
    .OnePastLastY = OnePastLastY,
  };
  return Result;
}
work_queue WorkQueueInit(tile_fmt TileFormat, u8 *ImageBuffer, u32 ImageWidth, u32 ImageHeight, world *World, u32 MaxDepth, camera *Camera, u32 SamplesPerPixel, u32 CoreCount)
{
  u32 TileWidth  = ImageWidth/CoreCount;
  u32 TileHeight = TileWidth;
  u32 TileCountX = ((ImageWidth  + TileWidth ) - 1)/(TileWidth);
  u32 TileCountY = ((ImageHeight + TileHeight) - 1)/(TileHeight);
  u32 TileTotalCount = TileCountX*TileCountY;
  work_queue Result = {
    .WorkOrders = (work_order *)OSMemoryAlloc(TileTotalCount*sizeof(work_order)),
    .WorkOrderCount = 0,
    .TileFormat = TileFormat,
    .TileWidth  = TileWidth,
    .TileHeight = ImageHeight,
    .TileCountX = TileCountX,
    .TileCountY = TileCountY,
    .TileTotalCount = TileTotalCount,
  };
  Assert(TileFormat==TileFmt_uint_R8G8B8A8 && "Must learn table generation");
  
  for(u32 TileY=0; TileY<TileCountY; TileY++)
  {
    u32 MinY         = TileY*TileHeight;
    u32 OnePastLastY = Min(MinY+TileHeight, ImageHeight);
    for(u32 TileX=0; TileX<TileCountX; TileX++)
    {
      u32 MinX         = TileX*TileWidth;
      u32 OnePastLastX = Min(MinX+TileHeight, ImageWidth);
      work_order *Order = &Result.WorkOrders[Result.WorkOrderCount++];
      work_order NewOrder = WorkOrderInit(ImageBuffer, ImageWidth, ImageHeight,
                                          World, MaxDepth, MinX, MinY,
                                          OnePastLastX, OnePastLastY,
                                          Camera, SamplesPerPixel);
      WriteToRef(Order, NewOrder);
    }
  }
  
  return Result;
}

#endif //RTWORK_H