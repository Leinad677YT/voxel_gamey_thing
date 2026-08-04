#pragma once

#include <SDL3/SDL.h>

struct world_data {
    struct entity_generic** entities;
    Uint32 entity_amount;
    size_t entity_max;

    struct entity_player** players;
    Uint32 player_amount;
    size_t player_max;
} WORLD = {
    .entities = NULL,
    .entity_amount = 0,
    .entity_max = 0,

    .players = NULL,
    .player_amount = 0,
    .player_max = 0
};