#ifndef RTMEMORY_H
#define RTMEMORY_H

#ifndef MEMORY_DEFAULT_ALIGNMENT
#define MEMORY_DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

typedef struct arena arena;
struct arena
{
  u8  *Base;
  u64  Size;
  u64  CurrOffset;
  u64  PrevOffset;
};
typedef struct arena_temp arena_temp;
struct arena_temp
{
	arena *Arena;
  u64 PrevOffset;
	u64 CurrOffset;
};
typedef arena_temp scratch;

#define TCTX_SCRATCH_POOL_COUNT 2
typedef struct thread_ctx thread_ctx;
struct thread_ctx
{ 
  u8   *Memory;
  arena ScratchPool[TCTX_SCRATCH_POOL_COUNT];
};

void MemoryCopy(void *DstBuffer, u64 DstSize, void *SrcBuffer, u64 SrcSize);
void *ArenaPushBlock(arena *Arena, u64 Size);
void ThreadCtxSet(void *Ptr);
void ThreadCtxInit(thread_ctx *Ctx, void *Memory, u64 MemorySize);
arena ArenaInit(arena *Arena, size_t Size, void *Address);

#define ArenaPushType(  Arena,        Type) (Type *)ArenaPushBlock(Arena, sizeof(Type))
#define ArenaPushArray( Arena, Count, Type) (Type *)ArenaPushBlock(Arena, (Count) * sizeof(Type))
#define ArenaZeroType(  Instance          ) ArenaZeroBlock(sizeof(Instance), &(Instance))
#define MemoryGetScratch(used_arena_list, used_arena_count) \
ThreadCtxGetScratch(ThreadCtxGet(), used_arena_list, used_arena_count);;
#define MemoryReleaseScratch(scratch) ArenaTempEnd(scratch);


#endif //RTMEMORY_H
