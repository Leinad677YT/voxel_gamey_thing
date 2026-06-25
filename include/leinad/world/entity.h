#pragma once

#include "../data/types.h"


struct entity_tag_node {
    char* tag;
    size_t len;
    struct entity_tag_node *next;
};

struct entity_passenger_node {
    struct entity * entity;
    struct entity_passenger_node *next;
};

typedef struct entity {
    uuid uuid;

    struct eNBT *data;
    struct text_component* name;

    struct entity_tag_node tags;
    struct entity_passenger_node passengers;
    
    struct leinad_position pos;
    struct leinad_motion motion;
    struct leinad_rotation rotation;
        
    double fall_distance;

    Sint32 health;
    Sint32 portal_cooldown;
    Sint32 ticks_frozen;

    Sint16 air;
    Sint16 fire;

    bool name_visible : 1;
    bool glowing : 1;
    bool visual_fire : 1;
    bool invulnerable : 1;
    bool no_gravity : 1;
    bool on_ground : 1;
    bool silent : 1;
    
} entity;