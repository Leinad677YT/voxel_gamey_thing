/**
 * @param variable variable name to add the amount to, must be inside (-360,360)
 * @param amount amount to add IN DEGREES, must be inside (-360,360)
 */
#define LEINAD_ANGLE_ADD(variable,amount) do { \
        variable +=amount; \
        ((variable >= 360) ? (variable-=360): variable); \
        ((variable < 0) ? (variable+=360) : variable ); \
    } while(0)
