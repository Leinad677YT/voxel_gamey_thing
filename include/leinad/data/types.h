#pragma once

#include <SDL3/SDL.h>


// cover all other type headers
#include "../type/vec3.h"
#include "../type/matrix4x4.h"
#include "../type/text.h"
#include "../type/enbt.h"


// generic

typedef Sint32 uuid[4];

struct namespaced_id {
    Uint32 namespace;
    uuid id;
};

struct namespace {
    char* namespace;
    uuid id;
};


// gameplay related

struct leinad_position {
    double x, y, z;
};

struct leinad_rotation {
    double yaw /*0*/, pitch /*1*/; // usually used inverted but for datapack compat it is what it is :c
};

struct leinad_motion {
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

