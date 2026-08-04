#include <leinad/math/trigonometry.h>

#include <leinad/player.h>
#include <leinad/world.h>
#include <leinad/app.h>

struct entity_player* client = NULL;
struct player_input client_inputs = {0};

int leinad_load_player_client() {
    client = SDL_malloc(sizeof(struct entity_player));
    if (client == NULL) goto failure;
    SDL_memset(client,0,sizeof(struct entity_player));


    client->is_client = true;

    client->may_fly = true;

    client->generic.motion = (struct leinad_motion){0,0,0};
    client->generic.pos = (struct leinad_position){0,0,0};
    client->generic.rotation = (struct leinad_rotation){0,0};

    client->fly_speed = 2;
    client->walk_speed = 0.1;

    if (!WORLD.player_max) {
        WORLD.players = SDL_calloc(2, sizeof(struct entity_player*));
    } else if (WORLD.player_amount +1 > WORLD.player_max) {
        void* new_array = SDL_realloc(WORLD.players,sizeof(struct entity_player*) * WORLD.player_amount);
        if (new_array != NULL) WORLD.players = new_array;
    }

    if (WORLD.players == NULL) {
        SDL_free(client);
        goto failure;
    }

    WORLD.players[WORLD.player_amount] = client;
    WORLD.player_amount++;

    success:
        return SDL_APP_CONTINUE;
    failure:
        return SDL_APP_FAILURE;
}

LEINAD_AUX struct leinad_position _calculate_new_player_pos(const struct entity_player* player) {
    struct leinad_position pos = leinad_entity_getpos(&player->generic);
    struct leinad_motion motion = leinad_entity_getmotion(&player->generic);

    if (player->is_client) {
        motion.x += player->fly_speed * (-sin(degree_to_radian(player->generic.rotation.yaw)) * (client_inputs.right - 2* client_inputs.left) + cos(degree_to_radian(player->generic.rotation.yaw)) * (client_inputs.front - 2* client_inputs.back));

        motion.z += player->fly_speed * (sin(degree_to_radian(player->generic.rotation.yaw)) * (client_inputs.front - 2* client_inputs.back) + cos(degree_to_radian(player->generic.rotation.yaw)) * (client_inputs.right - 2* client_inputs.left));
    }

    if (player->generic.on_ground && client_inputs.up) motion.y += leinad_entity_getjump(&player->generic);
    else if (player->may_fly) motion.y += player->fly_speed * (client_inputs.up - 2* client_inputs.down);
        

    if (player->may_fly) return (struct leinad_position) {
        pos.x += motion.x * (APP.current_ns - APP.previous_ns) / 100000000,
        pos.y += motion.y * (APP.current_ns - APP.previous_ns) / 100000000,
        pos.z += motion.z * (APP.current_ns - APP.previous_ns) / 100000000
    };
    else return (struct leinad_position) {
        pos.x += motion.x * (APP.current_ns - APP.previous_ns) / 100000000,
        pos.y += motion.y * (APP.current_ns - APP.previous_ns) / 100000000,
        pos.z += motion.z * (APP.current_ns - APP.previous_ns) / 100000000
    };

}


LEINAD_FCALL int leinad_update_players() {

    if (WORLD.players == NULL) return SDL_APP_CONTINUE;

    for (int i = 0; i < WORLD.player_amount; i++){
        struct entity_player* player = WORLD.players[i];

        player->generic.pos = _calculate_new_player_pos(player);

        
    }

    return SDL_APP_CONTINUE;
}