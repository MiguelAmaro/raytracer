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

void MemoryCopy(void *DstBuffer, u64 DstSize, void *SrcBuffer, u64 SrcSize);
void *ArenaPushBlock(arena *Arena, u64 Size);



#define ArenaPushType(  Arena,        Type) (Type *)ArenaPushBlock(Arena, sizeof(Type))
#define ArenaPushArray( Arena, Count, Type) (Type *)ArenaPushBlock(Arena, (Count) * sizeof(Type))
#define ArenaZeroType(  Instance          ) ArenaZeroBlock(sizeof(Instance), &(Instance))

#define TCTX_SCRATCH_POOL_COUNT 2
typedef struct thread_ctx thread_ctx;
struct thread_ctx
{ 
  u8   *Memory;
  arena ScratchPool[TCTX_SCRATCH_POOL_COUNT];
};

#define GetScratchMemory(used_arena_list, used_arena_count) \
OSThreadCtxGetScratch(OSThreadCtxGet(gWin32ThreadContextId), \
used_arena_list, used_arena_count);



#endif //RTMEMORY_H
