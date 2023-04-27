#ifndef RTTEXTURE_H
#define RTTEXTURE_H

typedef enum texture_kind texture_kind;
enum texture_kind
{
  TextureKind_SolidColor,
};
typedef struct texture texture;
struct texture
{
  texture_kind Kind;
  v3f64 Color;
};

#endif //RTTEXTURE_H
