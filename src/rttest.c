
#define RT_FUNCTION

#define RTTEST_PASSED (1)
#define RTTEST_FAILED (0)

typedef enum rttest_colorid rttest_colorid;
enum rttest_colorid
{
  RTTestColor_Red     = 0,
  RTTestColor_Green   = 1,
  RTTestColor_Yellow  = 2,
  RTTestColor_Blue    = 3,
  RTTestColor_Magenta = 4,
  RTTestColor_Cyan    = 5,
  RTTestColor_White   = 6,
};
const char *RTTestStatusStrTable[] = {
  "FAILED",
  "PASSED",
};
const char *RTTestColorTable[] =
{
  "\x1b[31m", //ColorRed
  "\x1b[32m", //ColorGreen
  "\x1b[33m", //ColorYellow
  "\x1b[34m", //ColorBlue
  "\x1b[35m", //ColorMagenta
  "\x1b[96m", //ColorCyan
  "\x1b[0m"   //ColorWhite
};

#define RTTEST(function_call, expected_value, comment) \
_Generic((expected_value), \
f64: RTTestF64, v3f64: RTTestV3F64)(function_call, expected_value, #function_call, comment)

//~
RT_FUNCTION void RTTestF64(f64 Result, f64 Expected, const char *FuncName, const char *Comment)
{
  b32 TestStatus  = (Result == Expected);
  const char *StatusColor = (TestStatus==RTTEST_PASSED?
                             RTTestColorTable[RTTestColor_Green]:
                             RTTestColorTable[RTTestColor_Red]);
  printf("%s", StatusColor);
  printf("Testing: %s ==> %s!!!\n"
         "Result: %f %sExpected: %f\n",
         FuncName,
         RTTestStatusStrTable[TestStatus],  Result,
         RTTestColorTable[RTTestColor_Cyan], Expected);
  printf("%s", RTTestColorTable[RTTestColor_White]);
  printf("%s\n", Comment);
  return; 
}
RT_FUNCTION void RTTestV3F64(v3f64 Result, v3f64 Expected, const char *FuncName, const char *Comment)
{
  b32 TestStatus  = ((Result.x == Expected.x) &&
                     (Result.y == Expected.y) &&
                     (Result.z == Expected.z));
  const char *StatusColor = (TestStatus==RTTEST_PASSED?
                             RTTestColorTable[RTTestColor_Green]:
                             RTTestColorTable[RTTestColor_Red]);
  printf("%s", StatusColor);
  printf("Testing: %s ==> %s!!!\n"
         "Result: x:%f y:%f z:%f %sExpected: x:%f y:%f z:%f\n",
         FuncName,
         RTTestStatusStrTable[TestStatus], Result.x, Result.y, Result.z,
         RTTestColorTable[RTTestColor_Cyan], Expected.x, Expected.y, Expected.z);
  printf("%s", RTTestColorTable[RTTestColor_White]);
  printf("%s\n", Comment);
  return; 
}

RT_FUNCTION b32 RTTest_F64Equals(f64 Expected, f64 X)
{
  b32 Res = (X == Expected);
  return Res;
}
RT_FUNCTION b32 Test_F64IsNaN(f64 X)
{
  b32 Res = (X != X);
  return Res;
}

void TestingDone() { printf("testing done..."); }