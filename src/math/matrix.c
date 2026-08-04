#include <SDL3/SDL.h>

#include <leinad/type/matrix4x4.h>

matrix4x4 Matrix4x4_Multiply(matrix4x4 matrix1, matrix4x4 matrix2) {
    matrix4x4 result;

    result.m11 = (
        (matrix1.m11 * matrix2.m11) +
        (matrix1.m12 * matrix2.m21) +
        (matrix1.m13 * matrix2.m31) +
        (matrix1.m14 * matrix2.m41)
    );
    result.m12 = (
        (matrix1.m11 * matrix2.m12) +
        (matrix1.m12 * matrix2.m22) +
        (matrix1.m13 * matrix2.m32) +
        (matrix1.m14 * matrix2.m42)
    );
    result.m13 = (
        (matrix1.m11 * matrix2.m13) +
        (matrix1.m12 * matrix2.m23) +
        (matrix1.m13 * matrix2.m33) +
        (matrix1.m14 * matrix2.m43)
    );
    result.m14 = (
        (matrix1.m11 * matrix2.m14) +
        (matrix1.m12 * matrix2.m24) +
        (matrix1.m13 * matrix2.m34) +
        (matrix1.m14 * matrix2.m44)
    );
    result.m21 = (
        (matrix1.m21 * matrix2.m11) +
        (matrix1.m22 * matrix2.m21) +
        (matrix1.m23 * matrix2.m31) +
        (matrix1.m24 * matrix2.m41)
    );
    result.m22 = (
        (matrix1.m21 * matrix2.m12) +
        (matrix1.m22 * matrix2.m22) +
        (matrix1.m23 * matrix2.m32) +
        (matrix1.m24 * matrix2.m42)
    );
    result.m23 = (
        (matrix1.m21 * matrix2.m13) +
        (matrix1.m22 * matrix2.m23) +
        (matrix1.m23 * matrix2.m33) +
        (matrix1.m24 * matrix2.m43)
    );
    result.m24 = (
        (matrix1.m21 * matrix2.m14) +
        (matrix1.m22 * matrix2.m24) +
        (matrix1.m23 * matrix2.m34) +
        (matrix1.m24 * matrix2.m44)
    );
    result.m31 = (
        (matrix1.m31 * matrix2.m11) +
        (matrix1.m32 * matrix2.m21) +
        (matrix1.m33 * matrix2.m31) +
        (matrix1.m34 * matrix2.m41)
    );
    result.m32 = (
        (matrix1.m31 * matrix2.m12) +
        (matrix1.m32 * matrix2.m22) +
        (matrix1.m33 * matrix2.m32) +
        (matrix1.m34 * matrix2.m42)
    );
    result.m33 = (
        (matrix1.m31 * matrix2.m13) +
        (matrix1.m32 * matrix2.m23) +
        (matrix1.m33 * matrix2.m33) +
        (matrix1.m34 * matrix2.m43)
    );
    result.m34 = (
        (matrix1.m31 * matrix2.m14) +
        (matrix1.m32 * matrix2.m24) +
        (matrix1.m33 * matrix2.m34) +
        (matrix1.m34 * matrix2.m44)
    );
    result.m41 = (
        (matrix1.m41 * matrix2.m11) +
        (matrix1.m42 * matrix2.m21) +
        (matrix1.m43 * matrix2.m31) +
        (matrix1.m44 * matrix2.m41)
    );
    result.m42 = (
        (matrix1.m41 * matrix2.m12) +
        (matrix1.m42 * matrix2.m22) +
        (matrix1.m43 * matrix2.m32) +
        (matrix1.m44 * matrix2.m42)
    );
    result.m43 = (
        (matrix1.m41 * matrix2.m13) +
        (matrix1.m42 * matrix2.m23) +
        (matrix1.m43 * matrix2.m33) +
        (matrix1.m44 * matrix2.m43)
    );
    result.m44 = (
        (matrix1.m41 * matrix2.m14) +
        (matrix1.m42 * matrix2.m24) +
        (matrix1.m43 * matrix2.m34) +
        (matrix1.m44 * matrix2.m44)
    );

    return result;
}


matrix4x4 Matrix4x4_CreateTranslation(float x, float y, float z) {
    return (matrix4x4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    };
}

matrix4x4 Matrix4x4_CreatePerspectiveFieldOfView(
    float fieldOfView,
    float aspectRatio,
    float nearPlaneDistance,
    float farPlaneDistance
) {
    float num = 1.0f / ((float) SDL_tanf(fieldOfView * 0.5f));
    return (matrix4x4) {
        num / aspectRatio, 0, 0, 0,
        0, num, 0, 0,
        0, 0, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1,
        0, 0, (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance), 0
    };
}

matrix4x4 Matrix4x4_CreateLookAt(
    vec3 cameraPosition,
    vec3 cameraTarget,
    vec3 cameraUpVector
) {
    vec3 targetToPosition = {
        cameraPosition.x - cameraTarget.x,
        cameraPosition.y - cameraTarget.y,
        cameraPosition.z - cameraTarget.z
    };
    vec3 vectorA = vec3_normalize(targetToPosition);
    vec3 vectorB = vec3_normalize(vec3_cross(cameraUpVector, vectorA));
    vec3 vectorC = vec3_cross(vectorA, vectorB);

    return (matrix4x4) {
        vectorB.x, vectorC.x, vectorA.x, 0,
        vectorB.y, vectorC.y, vectorA.y, 0,
        vectorB.z, vectorC.z, vectorA.z, 0,
        -vec3_dot(vectorB, cameraPosition), -vec3_dot(vectorC, cameraPosition), -vec3_dot(vectorA, cameraPosition), 1
    };
}

vec3 vec3_normalize(vec3 vec) {
    float magnitude = SDL_sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
    return (vec3){
        vec.x / magnitude,
        vec.y / magnitude,
        vec.z / magnitude
    };
}

float vec3_dot(vec3 vecA, vec3 vecB) {
    return (vecA.x * vecB.x) + (vecA.y * vecB.y) + (vecA.z * vecB.z);
}

vec3 vec3_cross(vec3 vecA, vec3 vecB) {
    return (vec3) {
        vecA.y * vecB.z - vecB.y * vecA.z,
        -(vecA.x * vecB.z - vecB.x * vecA.z),
        vecA.x * vecB.y - vecB.x * vecA.y
    };
}

vec3 vec3_add_scaled(vec3 vecA, vec3 vecB, float scaleB) {
    return (vec3) {
        vecA.x + vecB.x * scaleB,
        vecA.y + vecB.y * scaleB,
        vecA.z + vecB.z * scaleB
    };
}