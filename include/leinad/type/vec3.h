#pragma once

typedef struct vec3 {
    float x, y, z;
} vec3;

vec3 Vec3_Normalize(vec3 vec);
float Vec3_Dot(vec3 vecA, vec3 vecB);
vec3 Vec3_Cross(vec3 vecA, vec3 vecB);