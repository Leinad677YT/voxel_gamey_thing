#include <leinad/player.h>

struct entity_player* client = NULL;

int leinad_load_player_client() {
    client = SDL_malloc(sizeof(struct entity_player));
    if (client == NULL) goto failure;
    SDL_memset(client,0,sizeof(struct entity_player));


    client->is_client = true;

    client->generic.motion = (struct leinad_motion){0,0,0};
    client->generic.pos = (struct leinad_position){0,0,0};

    client->fly_speed = 5;
    client->walk_speed = 0.1;

    success:
        return SDL_APP_CONTINUE;
    failure:
        return SDL_APP_FAILURE;
}