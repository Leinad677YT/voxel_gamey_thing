#pragma once

typedef struct vec3 {
    float x, y, z;
} vec3;

vec3 vec3_normalize(vec3 vec);
float vec3_dot(vec3 vecA, vec3 vecB);
vec3 vec3_cross(vec3 vecA, vec3 vecB);
vec3 vec3_add_scaled(vec3 vecA, vec3 vecB, float scaleB);