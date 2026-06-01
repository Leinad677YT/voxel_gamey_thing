#pragma once

#include <SDL3/SDL.h>

// gameplay related

struct leinad_position {
    double x, y, z;
};














// generic vertex data

typedef struct PositionVertex
{
	float x, y, z;
} PositionVertex;

typedef struct PositionColorVertex
{
	float x, y, z;
	Uint8 r, g, b, a;
} PositionColorVertex;

typedef struct PositionTextureVertex
{
    float x, y, z;
    float u, v;
} PositionTextureVertex;

typedef struct PositionTextureColorVertex
{
    float x, y, z;
    float u, v;
    float r,g,b,a;
} PositionTextureColorVertex;

