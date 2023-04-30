#ifndef RTTYPES_H
#define RTTYPES_H


#define Assert(expression) if(!(expression)){ __debugbreak(); } while (0)
#define ArrayCount(array) sizeof(array)/sizeof(array[0])
#define WriteToRef(refptr, obj) (*refptr = obj)
#define ObjCopyFromRef(type, ptr) (*((type *)ptr))

#define Kilobytes(size) (         (size) * 1024LL)
#define Megabytes(size) (Kilobytes(size) * 1024LL)
#define Gigabytes(size) (Megabytes(size) * 1024LL)
#define Terabytes(size) (Gigabytes(size) * 1024LL)


typedef uint8_t   u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint32_t b32;
typedef  int32_t s32;
typedef   double f64;
#define U32Max UINT32_MAX;

// USEFUL FUNCTIONS
inline f64 Inifintyf64(void)
{
  u64 u = 0x7ff0000000000000;
  f64 *p = (f64*)&u;
  return(*p);
}
inline u32 SafeTruncateu64(u64 Value)
{
  Assert(Value <= 0xffffffff);
  u32 Result = (u32)Value;
  return Result;
}
inline b32 IsPowerOfTwo(u64 a)
{
  b32 Result = (a & (a-1)) == 0;
  return Result;
}
#endif //RTTYPES_H
