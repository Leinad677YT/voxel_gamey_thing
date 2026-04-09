#pragma once

typedef struct matrix4x4 {
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
} matrix4x4;

typedef struct vec3 {
    float x, y, z;
} vec3;

matrix4x4 Matrix4x4_Multiply(matrix4x4 matrix1, matrix4x4 matrix2);
matrix4x4 Matrix4x4_CreateTranslation(float x, float y, float z);
matrix4x4 Matrix4x4_CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
matrix4x4 Matrix4x4_CreateLookAt(vec3 cameraPosition, vec3 cameraTarget, vec3 cameraUpVector);

vec3 Vec3_Normalize(vec3 vec);
float Vec3_Dot(vec3 vecA, vec3 vecB);
vec3 Vec3_Cross(vec3 vecA, vec3 vecB);