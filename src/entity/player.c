#include <leinad/data/types.h>
#include <leinad/world/entity.h>

#define MAX_ECHEST_SIZE 27
#define MAX_INVENTORY_SIZE 36




struct entity_player {
    struct entity entity;
    struct entity_alive alive;
    struct entity_hurt hurt;
    struct entity_equipment equipment;

    struct namespaced_id dimension; // only present on players beacause of not saving on regions
    Uint32 selected_item_slot;

    struct item_stack* ender_items[MAX_ECHEST_SIZE];
    struct item_stack* inventory[MAX_INVENTORY_SIZE];

    Sint32 food_level;
    Sint32 food_tick_timer;
    float food_saturation_level;
    float food_exhaustion_level;

    Sint32 xp_total;
    Sint32 xp_level;
    float xp_progress;
    
    struct {
        struct namespaced_id dimension;
        Sint32 pos[3];
    } last_death_location;

    struct {
        struct namespaced_id dimension;
        Sint32 pos;
        float yaw;
        float pitch;

    } respawn;
    
    struct entity* shoulder_entity_left;
    struct entity* shoulder_entity_right;

    struct entity* root_vehicle;
    uuid mounted_vehicle;

    // abilities
        float fly_speed;
        float walk_speed;
    //
    
    Sint32 gamemode;
    Sint32 previous_gamemode;

    //
        bool invulnerable : 1; // gets ignored by other entities as well
        bool flying : 1;
        bool instabuild : 1;
        bool may_build : 1;
        bool may_fly : 1;
    //

    bool seen_credits: 1;
    bool forced_respawn : 1;

    bool is_client : 1; // only used on the client

};

