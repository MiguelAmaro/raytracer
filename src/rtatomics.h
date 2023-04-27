#ifndef RTATOMICS_H
#define RTATOMICS_H

#include <windows.h>

u64 LockedAddAndGetLastValue(u64 volatile *Value, u64 Addend)
{
  u64 Result = InterlockedExchangeAdd64((volatile LONG64 *)Value, Addend);
  return Result;
}

#endif //RTATOMICS_H
