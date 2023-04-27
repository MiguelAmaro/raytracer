#ifndef RTRAY_H
#define RTRAY_H

typedef struct ray ray;
struct ray
{
  v3f64 Origin;
  v3f64 Dir;
  f64 Time;
};
ray RayInit(v3f64 Origin, v3f64 Dir, f64 Time);

#endif //RTRAY_H
