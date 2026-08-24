/**
 */

#include <leinad/data/control_shortcuts.h>
#include <leinad/data/namespacing.h>

#include <assert.h>

#include "_load_resource.c"

static SDL_EnumerationResult single_namespace_load(void* userdata, const char *dirname, const char *fname) {

    struct _load_namespaces *data = userdata;
    char* path = NULL;
    SDL_PathInfo info;
    
    struct _aux_namespace* current_namespace = NULL;

    // NOT A DIRECTORY, RETURN

    if (SDL_asprintf(&path, "%s/%s/", dirname, fname) < 0) goto failure;

    if (!SDL_GetPathInfo(path,NULL)) return SDL_ENUM_CONTINUE;


    // GET CURRENT NAMESPACE

    current_namespace = data->start;
    while (current_namespace != NULL) {
        if (!SDL_strcmp(fname, current_namespace->name_str)) break;
    }


    // IF NOT FOUND, CREATE TEMPORARY NAMESPACE
    
    if (current_namespace == NULL) { // is it the first one to load

        current_namespace = SDL_malloc(sizeof(struct _aux_namespace));

        if (current_namespace == NULL) goto failure;
        
        for (int i = 0; i < LEINAD_RESOURCETYPE_amount; i++) {
            current_namespace->ids[i] = NULL;
            current_namespace->id_amounts[i] = 0;
        }

        current_namespace->name_str = NULL;
        current_namespace->name_len = SDL_asprintf(&current_namespace->name_str, "%s",fname);

        if (current_namespace->name_len < 0) goto failure;

        current_namespace->next = data->start;
        data->start = current_namespace;
    }

    #define _load_type(_type,_location) { \
        data->type = _type ; \
        SDL_free(path); \
        if (SDL_asprintf(&path, "%s%s/" _location, dirname, fname) < 0) goto failure; \
        if (SDL_GetPathInfo(path,&info) && info.type == SDL_PATHTYPE_DIRECTORY) if (!SDL_EnumerateDirectory(path,resource_load,userdata)) goto failure; \
    }
    #define _START 63
    static_assert(__LINE__ == _START,__FILE__": MOVED RESOURCETYPE LOAD START");
    _load_type(LEINAD_RESOURCETYPE_FUNCTION, "function");
    _load_type(LEINAD_RESOURCETYPE_STRUCTURE, "structure");
    _load_type(LEINAD_RESOURCETYPE_ADVANCEMENT, "advancement");
    _load_type(LEINAD_RESOURCETYPE_BANNER_PATTERN, "banner_pattern");
    _load_type(LEINAD_RESOURCETYPE_CAT_VARIANT, "cat_variant");
    _load_type(LEINAD_RESOURCETYPE_CHAT_TYPE, "chat_type");
    _load_type(LEINAD_RESOURCETYPE_CHICKEN_VARIANT, "chicken_variant");
    _load_type(LEINAD_RESOURCETYPE_COW_VARIANT, "cow_variant");
    _load_type(LEINAD_RESOURCETYPE_DAMAGE_TYPE, "damage_type");
    _load_type(LEINAD_RESOURCETYPE_DECORATED_POT_PATTERN, "decorated_pot_pattern");
    _load_type(LEINAD_RESOURCETYPE_DIALOG,"dialog");
    _load_type(LEINAD_RESOURCETYPE_DIMENSION,"dimension");
    _load_type(LEINAD_RESOURCETYPE_DIMENSION_TYPE,"dimension_type");
    _load_type(LEINAD_RESOURCETYPE_ENCHANTMENT,"enchantment");
    _load_type(LEINAD_RESOURCETYPE_ENCHANTMENT_PROVIDER,"enchantment_provider");
    _load_type(LEINAD_RESOURCETYPE_FROG_VARIANT,"frog_variant");
    _load_type(LEINAD_RESOURCETYPE_INSTRUMENT,"instrument");
    _load_type(LEINAD_RESOURCETYPE_ITEM_MODIFIER,"item_modifier");
    _load_type(LEINAD_RESOURCETYPE_JUKEBOX_SONG,"jukebox_song");
    _load_type(LEINAD_RESOURCETYPE_LOOT_TABLE,"loot_table");
    _load_type(LEINAD_RESOURCETYPE_NUMBER_PROVIDER,"number_provider");
    _load_type(LEINAD_RESOURCETYPE_PAINTING_VARIANT,"painting_variant");
    _load_type(LEINAD_RESOURCETYPE_PIG_VARIANT,"pig_variant");
    _load_type(LEINAD_RESOURCETYPE_PREDICATE,"predicate");
    _load_type(LEINAD_RESOURCETYPE_RECIPE,"recipe");
    _load_type(LEINAD_RESOURCETYPE_SLOT_SOURCE,"slot_source");
    _load_type(LEINAD_RESOURCETYPE_SULFUR_CUBE_ARCHETYPE,"sulfur_cube_archetype");
    _load_type(LEINAD_RESOURCETYPE_TEST_ENVIROMENT,"test_enviroment");
    _load_type(LEINAD_RESOURCETYPE_TEST_INSTANCE,"test_instance");
    _load_type(LEINAD_RESOURCETYPE_TIMELINE,"timeline");
    _load_type(LEINAD_RESOURCETYPE_TRADE_SET,"trade_set");
    _load_type(LEINAD_RESOURCETYPE_TRIAL_SPAWNER,"spawner");
    _load_type(LEINAD_RESOURCETYPE_TRIM_MATERIAL,"trim_material");
    _load_type(LEINAD_RESOURCETYPE_TRIM_PATTERN,"trim_pattern");
    _load_type(LEINAD_RESOURCETYPE_VILLAGER_TRADE,"villager_trade");
    _load_type(LEINAD_RESOURCETYPE_WOLF_SOUND_VARIANT,"wolf_sound_variant");
    _load_type(LEINAD_RESOURCETYPE_WOLF_VARIANT,"wolf_variant");
    _load_type(LEINAD_RESOURCETYPE_WORLD_CLOCK,"world_clock");
    _load_type(LEINAD_RESOURCETYPE_ZOMBIE_NAUTILUS_VARIANT,"zombie_nautilus_variant");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_BIOME,"worldgen/biome");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_CONFIGURED_CARVER,"worldgen/configured_carver");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_CONFIGURED_FEATURE,"worldgen/configured_feature");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_DENSITY_FUNCTION,"worldgen/density_function");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_NOISE,"worldgen/noise");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_NOISE_SETTINGS,"worldgen/noise_settings");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_PLACED_FEATURE,"worldgen/placed_feature");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_PROCESSOR_LIST,"worldgen/processor_list");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_STRUCTURE,"worldgen/structure");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_STRUCTURE_SET,"worldgen/structure_set");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_TEMPLATE_POOL,"worldgen/template_pool");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_WORLD_PRESET,"worldgen/world_preset");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_FLAT_LEVEL_GENERATOR_PRESET,"worldgen/flat_level_generator_preset");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_MULTI_NOISE_BIOME_SOURCE_PARAMETER_LIST,"worldgen/multi_noise_biome_source_parameter_list");
    static_assert(__LINE__ - _START == LEINAD_RESOURCETYPE_end_vanilla,__FILE__":%d FORGOT RESOURCETYPE LOAD ELEMENTS");

    #undef _load_type
    #define _load_type(_type,_location) { \
        data->type = _type ; \
        SDL_free(path); \
        if (SDL_asprintf(&path, "%s%s/tags" _location, dirname, fname) < 0) goto failure; \
        if (SDL_GetPathInfo(path,&info) && info.type == SDL_PATHTYPE_DIRECTORY) if (!SDL_EnumerateDirectory(path,tag_load,userdata)) goto failure; \
    }

    _load_type(LEINAD_RESOURCETYPE_BANNER_PATTERN, "banner_pattern");
    _load_type(LEINAD_RESOURCETYPE_BLOCK, "block");
    _load_type(LEINAD_RESOURCETYPE_DAMAGE_TYPE, "damage_type");
    _load_type(LEINAD_RESOURCETYPE_DIALOG, "dialog");
    _load_type(LEINAD_RESOURCETYPE_DIALOG, "enchantment");
    _load_type(LEINAD_RESOURCETYPE_ENTITY_TYPE, "entity_type");
    _load_type(LEINAD_RESOURCETYPE_FLUID, "fluid");
    _load_type(LEINAD_RESOURCETYPE_FUNCTION, "function");
    _load_type(LEINAD_RESOURCETYPE_GAME_EVENT, "game_event");
    _load_type(LEINAD_RESOURCETYPE_INSTRUMENT, "instrument");
    _load_type(LEINAD_RESOURCETYPE_ITEM, "item");
    _load_type(LEINAD_RESOURCETYPE_PAINTING_VARIANT, "painting_variant");
    _load_type(LEINAD_RESOURCETYPE_POINT_OF_INTEREST_TYPE, "point_of_interest_type");
    _load_type(LEINAD_RESOURCETYPE_POTION, "potion");
    _load_type(LEINAD_RESOURCETYPE_TIMELINE, "timeline");
    _load_type(LEINAD_RESOURCETYPE_VILLAGER_TRADE, "villager_trade");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_BIOME,"worldgen/biome");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_CONFIGURED_FEATURE,"worldgen/feature");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_FLAT_LEVEL_GENERATOR_PRESET,"worldgen/flat_level_generator_preset");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_STRUCTURE,"worldgen/structure");
    _load_type(LEINAD_RESOURCETYPE_WORLDGEN_WORLD_PRESET,"worldgen/world_preset");

    #undef _START
    #undef _load_type

    return SDL_ENUM_CONTINUE;
  failure:
    SDL_free(path);
    return SDL_ENUM_FAILURE;

}



static int validate_packmcmeta (struct _load_namespaces *data, const char* pack_name) {

    char* path = NULL;
    SDL_PathInfo info;
    int status = 0, line = -1;

    if (SDL_asprintf(&path, "%s/%s", data->root_directory, pack_name) < 0) {status = -1; line = __LINE__; goto fail;}

    if (!SDL_GetPathInfo(path,&info)) {status = -1; line = __LINE__; goto fail;} // this should **never** happen

    if (info.type != SDL_PATHTYPE_DIRECTORY) {status = 1; goto fail;}

    SDL_free(path);
    if (SDL_asprintf(&path, "%s/%s/pack.mcmeta", data->root_directory, pack_name) < 0) {status = -1; line = __LINE__; goto fail;}

    if (!SDL_GetPathInfo(path,&info)) {status = 2; goto fail;}

    if (info.type != SDL_PATHTYPE_FILE) {status = 3; goto fail;}


  success:
    SDL_Log("Successfully loaded %s as a datapack\n",pack_name);
    SDL_free(path);
    return LEINAD_RETURN_SUCCESS;
  fail:
    SDL_Log("Failed to load %s as a datapack",pack_name);
    switch (status){
        case -1:
            SDL_Log("> [!] UNEXPECTED ERROR at %s:%d\n - %s",__FILE_NAME__, line,SDL_GetError());
            SDL_Log("> When reading %s: %s",path,SDL_GetError());
            break;
        case 1:
            SDL_Log("> %s is a file, not a directory!",path);
            break;
        case 2:
            SDL_Log("> %s/%s does not contain a pack.mcmeta",data->root_directory,pack_name);
            break;
        case 3:
            SDL_Log("> %s is not a file",path);
            break;
    }
    SDL_free(path);
    return LEINAD_RETURN_FAILURE;
}

static SDL_EnumerationResult single_datapack_load(void *userdata, const char *dirname, const char *fname) {

    char* path = NULL;

    if (validate_packmcmeta(userdata, fname) == LEINAD_RETURN_FAILURE) goto end;

    if (SDL_asprintf(&path, "%s%s/data/", dirname, fname) < 0) goto failure;
    if (!SDL_GetPathInfo(path,NULL)) goto end;
    
    if (!SDL_EnumerateDirectory(path,single_namespace_load,userdata)) goto failure;

  end:
    SDL_free(path);
    return SDL_ENUM_CONTINUE;
  failure:
    SDL_free(path);
    return SDL_ENUM_FAILURE;
}

static int load_all() {

    char *root_path = NULL;

    if (SDL_asprintf(&root_path, "%sdatapacks", SDL_GetBasePath()) < 0) return SDL_ENUM_FAILURE;

    if (!SDL_GetPathInfo(root_path,NULL)) {
        SDL_Log("Impossible to load datapacks. Cannot find %s",root_path);
        return LEINAD_RETURN_CONTINUE;
    }

    SDL_Log("root path is: %s",root_path);

    struct _load_namespaces data = {
        .root_directory = root_path,
        .start = NULL
    };
    
    if (!SDL_EnumerateDirectory(root_path,single_datapack_load,&data)) return LEINAD_RETURN_FAILURE;

    SDL_free(root_path);
    return LEINAD_RETURN_CONTINUE;
}


/*
 data
File directory.png: Sprite image for directory in Minecraft tags
    X File directory.png: Sprite image for directory in Minecraft banner_pattern
    X File directory.png: Sprite image for directory in Minecraft block
    X File directory.png: Sprite image for directory in Minecraft damage_type
    File directory.png: Sprite image for directory in Minecraft dialog
    File directory.png: Sprite image for directory in Minecraft enchantment
    File directory.png: Sprite image for directory in Minecraft entity_type
    File directory.png: Sprite image for directory in Minecraft fluid
    File directory.png: Sprite image for directory in Minecraft function
    File directory.png: Sprite image for directory in Minecraft game_event
    File directory.png: Sprite image for directory in Minecraft instrument
    File directory.png: Sprite image for directory in Minecraft item
    File directory.png: Sprite image for directory in Minecraft painting_variant
    File directory.png: Sprite image for directory in Minecraft point_of_interest_type
    File directory.png: Sprite image for directory in Minecraft potion
    File directory.png: Sprite image for directory in Minecraft timeline
    File directory.png: Sprite image for directory in Minecraft villager_trade
    X    File directory.png: Sprite image for directory in Minecraft biome
    X    File directory.png: Sprite image for directory in Minecraft feature​[upcoming: JE 26.3]
    X    File directory.png: Sprite image for directory in Minecraft flat_level_generator_preset
    X    File directory.png: Sprite image for directory in Minecraft structure
    X    File directory.png: Sprite image for directory in Minecraft world_preset

*/