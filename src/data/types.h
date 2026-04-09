#pragma once

#include <SDL3/SDL.h>

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

