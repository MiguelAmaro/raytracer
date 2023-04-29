
void MemoryCopy(void *DstBuffer, u64 DstSize, void *SrcBuffer, u64 SrcSize)
{ 
  u8* Src = (u8 *)SrcBuffer;
  u8* Dst = (u8 *)DstBuffer;
  for(u32 Index = 0; Index < SrcSize && Index < DstSize;
      Index++, Src++, Dst++)
  { *Dst = *Src; }
  return;
}
void MemorySet(u32 Value, void *Memory, u64 Size)
{
  u8 *Dest = (u8 *)Memory;
  while(Size--) {*Dest++ = (u8)Value;}
  return;
}
u64 MemoryAlignFoward(u64 Address, u64 Align)
{
  u64 Result = Address;
  Assert(IsPowerOfTwo(Align));
  u64 Mod = Address & (Align - 1);
  if(Mod != 0) Result += Align - Mod;
  return Result;
}

//~ ARENAS
arena ArenaInit(arena *Arena, size_t Size, void *Address)
{
  arena Result =
  {
    .Base = Address,
    .Size = Size,
    .CurrOffset = 0,
    .PrevOffset = 0,
  };
  if(Arena) { *Arena = Result; }
  return Result;
}
void ArenaReset(arena *Arena)
{
  MemorySet(0, Arena->Base, Arena->CurrOffset);
  Arena->CurrOffset = 0;
  Arena->PrevOffset = 0;
  return;
}
void *ArenaPushBlock(arena *Arena, u64 Size)
{
  u64 NewBlock = (u64)Arena->Base + (u64)Arena->CurrOffset;
  u64 AlignedOffset = MemoryAlignFoward(NewBlock, MEMORY_DEFAULT_ALIGNMENT);
  AlignedOffset -= (u64)Arena->Base; // is now relative offset
  void *Result = NULL;
  if(AlignedOffset + Size <= (u64)Arena->Size)
  {
    Result = (Arena->Base + AlignedOffset);
    Arena->PrevOffset = AlignedOffset;
    Arena->CurrOffset = AlignedOffset + Size;
    MemorySet(0, Result, Size);
  }
  Assert(Result != NULL);
  return Result;
}
arena_temp ArenaTempBegin(arena *Arena)
{
  arena_temp Temp;
	Temp.Arena = Arena;
	Temp.PrevOffset = Arena->PrevOffset;
	Temp.CurrOffset = Arena->CurrOffset;
	return Temp;
}
void ArenaTempEnd(arena_temp Temp)
{
	Temp.Arena->PrevOffset = Temp.PrevOffset;
	Temp.Arena->CurrOffset = Temp.CurrOffset;
  return;
}

//~ THREAD CONTEXT
void ThreadCtxSet(void *Ptr)
{
  TlsSetValue(GlobalOSThreadContextId, Ptr);
  return;
}
thread_ctx *ThreadCtxGet(void)
{
  thread_ctx *Result = (thread_ctx *)TlsGetValue(GlobalOSThreadContextId);
  return Result;
}
arena_temp ThreadCtxGetScratch(thread_ctx *Ctx, arena **Conflicts, u64 ConflictCount)
{
  arena_temp Result = {0};
  arena *Scratch = Ctx->ScratchPool;
  for(u64 Idx=0; Idx< ArrayCount(Ctx->ScratchPool); Idx++, Scratch++)
  {
    b32 IsNotConflict = 1;
    arena **Conflict = Conflicts;
    for(u32 j=0; j<ConflictCount; j++, Conflict++)
    {
      if(Scratch == *Conflict) { IsNotConflict = 0; break; }
    }
    if(IsNotConflict)
    {
      Result = ArenaTempBegin(Scratch);
      break;
    }
  }
  Assert(Result.Arena->Base);
  return Result;
}
void ThreadCtxInit(thread_ctx *Ctx, void *Memory, u64 MemorySize)
{
  //TODO(MIGUEL): This is kind of scetch 
  arena *Scratch = Ctx->ScratchPool;
  u64 ArenaCount = ArrayCount(Ctx->ScratchPool);
  u64 PerArenaSize  = MemoryAlignFoward(MemorySize/ArenaCount, 8);
  u8 *End = (u8 *)Memory+MemorySize;
  for(u32 Idx=0; Idx<ArrayCount(Ctx->ScratchPool); Idx++, Scratch++)
  {
    u8 *ArenaMem = (u8 *)MemoryAlignFoward((u64)((u8 *)Memory + PerArenaSize*Idx), 8);
    u64 ArenaSize = Min(PerArenaSize, (u64)(End-ArenaMem));
    *Scratch = ArenaInit(NULL, ArenaSize, ArenaMem); 
  }
  return;
}