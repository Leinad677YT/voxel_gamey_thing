#pragma once

#include "../data/types.h"
#include "../world/item.h"


struct entity_tag_node {
    char* tag;
    size_t len;
    struct entity_tag_node *next;
};

struct entity_passenger_node {
    struct entity * entity;
    struct entity_passenger_node *next;
};

struct entity {
    struct namespaced_id id;    // type
    uuid uuid;                  // uuid inside world

    struct eNBT *data;          // arbitrary nbt
    struct text_component* name;// "visible name" of the entity

    struct entity_tag_node tags;            // list of tags the entity has
    struct entity_passenger_node passengers;// list of passengers the entity has
    
    struct leinad_position pos;     // <x,y,z> position of the entity
    struct leinad_motion motion;    // <x,y,z> distance to move over this tick
    struct leinad_rotation rotation;// <yaw, pitch> of the entity facing direction
        
    double fall_distance;   // amount of distance fallen

    Sint32 portal_cooldown;
    Sint16 fire;

    bool name_visible : 1;
    bool glowing : 1;
    bool visual_fire : 1;
    bool invulnerable : 1;
    bool no_gravity : 1;
    bool on_ground : 1;
    bool silent : 1;
    
} entity;

struct entity_alive{
    void* effects;
    void* attributes;
    void* leash;

    float health;
    float absorption;

    Sint32 sleeping_pos[3];

    Sint32 ticks_frozen;
    Sint16 air;
    
    Sint16 death_time;
    Sint16 hurt_time;

    bool can_pickup_loot : 1;
    bool no_ai : 1;
    bool persistant : 1;
    bool left_handed : 1;
    bool fall_flying : 1;
};

struct entity_home {
    Sint32 home_pos[3];
    Sint32 home_radius;
};

struct entity_hurt {
    uuid last_hurt_by_entity;
    uuid last_hurt_by_player;

    Sint32 last_player_hurt_ticks;
    Sint32 last_entity_hurt_ticks;
};

struct entity_equipment {
    struct item_stack* head;
    struct item_stack* chest;
    struct item_stack* legs;
    struct item_stack* feet;

    struct item_stack* mainhand;
    struct item_stack* offhand;

    struct item_stack* body;
    struct item_stack* saddle;
};