#pragma once

#define cos(x) SDL_cos(x)
#define sin(x) SDL_sin(x)

/**
 * @param variable variable name to add the amount to, must be inside (-360,360)
 * @param amount amount to add IN DEGREES, must be inside (-360,360)
 */
#define LEINAD_ANGLE_ADD(variable,amount) do { \
        variable +=amount; \
        ((variable >= 360) ? (variable-=360): variable); \
        ((variable < 0) ? (variable+=360) : variable ); \
    } while(0)

#define degree_to_radian(degree) ((degree) * SDL_PI_F / 180.0f)

/**
 * @param yaw
 * @param pitch
 * @return unitary vec3 that faces in the direction composed by both angles
 */
#define angles_to_vec3(yaw, pitch) (vec3){cos(pitch)*cos(yaw),sin(pitch),cos(pitch)*sin(yaw)}