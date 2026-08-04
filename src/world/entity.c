#include <leinad/world.h>
#include <leinad/app.h>
#include <leinad/world/entity.h>

LEINAD_AUX static struct leinad_position _calculate_new_pos(const struct entity_generic* entity) {
    struct leinad_position pos = leinad_entity_getpos(entity);
    struct leinad_motion motion = leinad_entity_getmotion(entity);

    return (struct leinad_position){
        pos.x += motion.x * (APP.current_ns - APP.previous_ns) / 100000000,
        pos.y += motion.y * (APP.current_ns - APP.previous_ns) / 100000000,
        pos.z += motion.z * (APP.current_ns - APP.previous_ns) / 100000000
    };
}


LEINAD_FCALL int leinad_update_entities() {

    if (WORLD.entities == NULL) return SDL_APP_CONTINUE;

    for (int i = 0; i < WORLD.entity_amount; i++){
        struct entity_generic* entity = WORLD.entities[i];

        struct leinad_position new_pos = _calculate_new_pos(entity);

    }

    return SDL_APP_CONTINUE;
}