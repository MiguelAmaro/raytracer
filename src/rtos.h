#ifndef RTOS_H
#define RTOS_H

#include <windows.h>

u8 *OSGetWorkingDir(u8 *Buffer, u64 Size)
{
  DWORD Result = GetCurrentDirectory(SafeTruncateu64(Size), (LPTSTR)Buffer);
  Assert(Result!=0);
  return Buffer;
}

//~ TIMER
typedef struct os_timer os_timer;
struct os_timer
{
  b32 IsInitialized;
  u64 Frequency;
};
os_timer GlobalOSTimer = {0};
void OSTimerInit(void)
{
  LARGE_INTEGER Freq = {0};
  QueryPerformanceFrequency(&Freq);
  GlobalOSTimer.Frequency = Freq.QuadPart;
  GlobalOSTimer.IsInitialized = 1;
}
u64 OSTimerGetTick(void)
{
  LARGE_INTEGER Tick = {0};
  QueryPerformanceCounter(&Tick);
  u64 Result = Tick.QuadPart;
  return Result;
}
f64 OSTimerGetSecondsElepsed(u64 StartTick, u64 EndTick)
{
  if(!GlobalOSTimer.IsInitialized) OSTimerInit();
  u64 Delta = EndTick-StartTick;
  f64 Result = (Delta*1.0)/(f64)GlobalOSTimer.Frequency;
  return Result;
}

//~ MEMORY
#define threadlocal __declspec(thread)
threadlocal u32 GlobalOSThreadContextId = 0;
void *OSMemoryReserve(u64 Size)
{
  void *Result = VirtualAlloc(0, Size, MEM_RESERVE, PAGE_READWRITE);
  return Result;
}
b32 OSMemoryCommit(void *Ptr, u64 Size)
{
  b32 Result = (VirtualAlloc(Ptr, Size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return Result;
}
void OSMemoryDecommit(void *Ptr, u64 Size)
{
  VirtualFree(Ptr, Size, MEM_DECOMMIT);
  return;
}
void OSMemoryRelease(void *Ptr)
{
  VirtualFree(Ptr, 0, MEM_DECOMMIT);
  return;
}
void *OSMemoryAlloc(u64 Size)
{
  void *Ptr = OSMemoryReserve(Size);
  Assert(OSMemoryCommit(Ptr, Size));
  return Ptr;
}
//~ ENTROPY
HCRYPTPROV GlobalOSEntropyProvider = 0;
void OSEntropyInit(void)
{
  CryptAcquireContextA(&GlobalOSEntropyProvider, 0, 0, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
  return;
}
void OSEntropyGen(void *Data, u64 Size)
{
  if(GlobalOSEntropyProvider==0) 
  {
    fprintf(stderr, "please initialize os entropy provider..\n");
    Assert(0);
  }
  if(!CryptGenRandom(GlobalOSEntropyProvider, SafeTruncateu64(Size), (BYTE *)Data))
  {
    fprintf(stderr, "entropy generation failed..\n");
  }
  return;
}
void OSEntropyRelease(void)
{
  CryptReleaseContext(GlobalOSEntropyProvider, 0);
  return;
}

//~ THREADS
typedef DWORD osthreadproc(void *);
u64 OSThreadCreate(void *Param, osthreadproc ThreadProc)
{
  DWORD ThreadId;
  HANDLE ThreadHandle = CreateThread(0, 0, ThreadProc, Param, 0, &ThreadId);
  return (u64)ThreadHandle;
}
void OSThreadKill(void)
{
  ExitThread(0);
  return;
}
void OSThreadSync(u64 *ThreadHandles, u32 ThreadCount)
{
  WaitForMultipleObjects(ThreadCount, (HANDLE *)ThreadHandles, TRUE, INFINITE);
  return;
}
u32 OSGetCoreCount(void)
{
  SYSTEM_INFO SystemInfo = {0};
  GetSystemInfo(&SystemInfo);
  u32 Result = SystemInfo.dwNumberOfProcessors;
  return Result;
}
#endif //RTOS_H
