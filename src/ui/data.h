#pragma once

#include <SDL3/SDL.h>

#include "../libs/sorted_list.h"

#include "../data/tags.h"

#include "elements.h"

#define LEINAD_WELEM_DATA_SIZE 32
#define LEINAD_WELEM_INSTANCE_DATA_SIZE 32

#define LEINAD_UI_STATUS_CONTINUE    0x01
#define LEINAD_UI_STATUS_SUCCESS     0x02
#define LEINAD_UI_STATUS_FAILURE     0x04
#define LEINAD_UI_STATUS_INTERCEPTED 0x08



// [[[ 01. DATA TYPES ]]]



/** 
 * Window element types, if not properly specified, the system will use
 *  `LEINAD_WELEM_default` to render the element
 *
 * @note To loop over the type list, use a a loop in the form of
 *  `for(i=LEINAD_WELEM_init;i < LEINAD_WELEM_end; i++)`, if it cant be found, then assume
 *  the LEINAD_WELEM_default type
 */
typedef enum {
    LEINAD_WELEM_init   = 0,
    LEINAD_WELEM_BUTTON = LEINAD_WELEM_init,
    LEINAD_WELEM_end,
    
    LEINAD_WELEM_default = LEINAD_WELEM_end,
    LEINAD_WELEM_TEXTURE = LEINAD_WELEM_default
} leinad_welem_type_t;

/** 
 * Window element layers inside a UI, to easily specify when something should be on top
 *  of something else without worrying about their insertion order, which still applies 
 *  when they have the same layer
 */
typedef enum {
    LEINAD_WELEM_ONTOP,

    LEINAD_WELEM_LAYER5,
    LEINAD_WELEM_LAYER4,
    LEINAD_WELEM_LAYER3,
    LEINAD_WELEM_LAYER2,
    LEINAD_WELEM_LAYER1,

    LEINAD_WELEM_BACKGROUND
} leinad_welem_layer_t;

/** 
 * Generic window element data, to place/retrieve from `./elements.c -> welem_data[]`,
 *  each window element id represents a single window element with it's own data, and
 *  should be created with a specific builder for one of the `leinad_welem_type_t` values.
 *
 * @note This, represents the generic properties of a welem, to use it on a UI you need
 *  to create an instance (`leinad_welem_instance_t`) of it. 
 * 
 * @see leinad_welem_instance_t
 */
typedef struct leinad_welem {
    int width, height;          // base dimensions of the welem
    leinad_welem_type_t type;   // type of the welem
    leinad_welem_id_t id;       // id for retrieving/setting data to instances
                                    // these are specified inside `elements.h`

    Uint8 _data[LEINAD_WELEM_DATA_SIZE]; // filler space to define extra behaviour
} leinad_welem_t;

/** 
 * Instance of a welem to place on UIs.
 *
 * @note This, represents an instance of a welem, which can then be placed inside UIs,
 *  to define the data of a welem id: @see leinad_welem_t 
 * 
 * @see leinad_welem_t
 */
typedef struct leinad_welem_instance {
    leinad_welem_id_t id;       // id for retrieving/setting data to instances
                                    // these are specified inside `elements.h`
    int x, y;                   // coordinates of the top-left corner INSIDE the UI
    leinad_welem_layer_t layer; // decides render/click order, relative to it's UI
    SDL_Texture* texture;       // texture to render the welem as, NULL for none
    
    Uint8 _data[LEINAD_WELEM_INSTANCE_DATA_SIZE]; // filler space to define extra behaviour
} leinad_welem_instance_t;

/** 
 * Generic UI data to later instanciate and use over a window (either for
 *  rendering or enabling interactions).
 * 
 * @note This, represents the generic properties of a UI, to use it on a window you need
 *  to create an instance (`leinad_ui_instance_t`) of it. 
 *
 * @see leinad_ui_instance_t
 */
typedef struct leinad_ui {
    leinad_slist_t welems;  // list of pointers to instanciated welems that this UI uses
    SDL_Surface* surface;   // surface of the resulting UI surface, for getting/setting data directly
    SDL_Renderer* renderer;   // renderer of the surface
    SDL_Texture* texture;   // texture of the resulting UI surface, for the global renderer
    int w,h;                // available area for the UI (will cut the rendering if outside from it)
    bool kidnaps_screen;    // if true, no interaction outside from this UI can be done
} leinad_ui_t;

/** 
 * Instance of a UI, containing it's position on the window and its scale if needed  
 *
 * @note This, represents an instance of a UI, to define the data of a welem id:
 *  @see leinad_ui_t 
 */
typedef struct leinad_ui_instance {
    leinad_ui_t* ui;    // pointer to the UI data that's gonna use, cannot be NULL
    int x, y;           // upper-left_corner coordinates on the window
    Uint8 scale;        // scale to apply to it's welems sizes and relative positions
} leinad_ui_instance_t;

/**
 * Generic data to use on clickable window elements
 */
struct leinad_clickable_data {
    int click_margin_x, click_margin_y; // Non-clickable margin (in pixels) of the welem, 
                                            // suffers from UI instance scale as well
    void (*action)(void*);              // Function to execute when the user clicks on it
};





// [[[ 02. UI ]]]



/** 
 * [BUILDER] Creates the data for a UI, which must be freed when you dont need it
 *  anymore with `leinad_destroy_ui()`
 * A UI contains window elements, which can contain a visual representation and
 *  allow for interactions with the screen in some way
 * @param w width of the UI in pixels, [0,w] is the horizontal range on which the
 *  window elements are allowed to be placed on
 * @param h height of the UI in pixels, [0,h] is the vertical range on which the
 *  window elements are allowed to be placed on 
 * @param kidnaps_screen true if the UI being active means that no interaction
 *  outside from it can be done, false otherwise
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 *
 * @see leinad_destroy_ui
 * @see leinad_destroy_ui_instance
 * @see leinad_instanciate_ui
 * @see leinad_set_active_ui
 * @see leinad_render_ui
 * @see leinad_push_ui
 * @see leinad_update_ui
 */
LEINAD_FBUILDER leinad_ui_t* leinad_create_ui(int w, int h, bool kidnaps_screen);

/** 
 * [CLEANER] Frees the data of a UI to release resources, making it no longer accesible
 * @param ui pointer to the UI
 *
 * @see leinad_create_ui
 * @see leinad_set_active_ui
 * @see leinad_render_ui
 * @see leinad_push_ui
 * @see leinad_update_ui
 */
LEINAD_FCLEANER void leinad_destroy_ui(leinad_ui_t* ui);

/** 
 * Updates the surface of the specified UI so that the next `leinad_render_ui()`
 *  reflects the visual changes
 * @param ui pointer to the UI you want to update
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 *
 * @warning Please note that the logical changes like click detection work regardless
 *  of the rendering state if the UI is marked as active.
 * 
 * @see leinad_render_ui()
 */
bool leinad_update_ui(leinad_ui_t* ui);

/** 
 * [CALL] Renders all UI layers over the window
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 */
LEINAD_FCALL bool leinad_render_ui();

/** 
 * [INITIALIZER] Makes a UI instance that can be later off freed by calling
 *  `leinad_destroy_ui_instance()` on it when it's no longer needed
 *
 * @param ui pointer to the UI you want to render   
 * @param x defines the left side of the UI
 * @param y defines the top side of the UI
 * @param scale Scale of the UI, must be at least 1
 * @return Pointer to the instance on success, NULL otherwise
 */
LEINAD_FINITIALIZER leinad_ui_instance_t* leinad_instanciate_ui(leinad_ui_t* ui, int x, int y, Uint8 scale);


/** 
 * [CLEANER] Frees the data of a UI instance to release resources, making it no
 *  longer accesible
 * @param ui pointer to the instance
 */
LEINAD_FCLEANER void leinad_destroy_ui_instance(leinad_ui_instance_t* ui);


/** 
 * [CALL] Marks a UI to be the one active, enabling it's screen interactions
 * @param ui pointer to the UI you want to render, must be properly instanciated 
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 *
 * @note A UI does not need to be rendered to be active, and neither is the other
 *  way around.
 */
LEINAD_FCALL bool leinad_set_active_ui(leinad_ui_instance_t* ui);

/** 
 * Pushes a UI to the "rendering stack", placing it over the other ones
 * @param ui pointer to the UI you want to render, must be properly instanciated 
 * @param x defines the left side of the UI
 * @param y defines the top side of the UI
 * @param scale Scale of the target UI, must be at least 1
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 *
 * @note A UI does not need to be rendered to be active, and neither is the other
 *  way around.
 */
bool leinad_push_ui(leinad_ui_instance_t* ui);

/**
 * [CALL] Tries to interact with the UI based off the current event. This happens
 *  before the normal event system, but after things like window resizing or exiting
 *
 * @param appstate grabbed from the SDL_AppEvent call
 * @param event grabbed from the SDL_AppEvent call
 *
 * @return `LEINAD_UI_STATUS_CONTINUE` when the ui allows the event handler to
 *  manage it.  
 *  `LEINAD_UI_STATUS_SUCCESS` if the app should exit with SUCCESS code.  
 *  `LEINAD_UI_STATUS_FAILURE` if the app should exit with SUCCESS code.  
 *  `LEINAD_UI_STATUS_INTERCEPTED` if the event should not manage the event as well.
 */
LEINAD_FCALL int manage_ui_interaction(void *appstate, SDL_Event* event);





// [[[ 03. WELEM ]]]



// defined inside every specific type header
    // LEINAD_FBUILDER leinad_ui_t* leinad_create_welem();

// defined inside every specific type header
    // LEINAD_FCLEANER void leinad_destroy_welem();

    // defined inside every specific type header
    // LEINAD_FINITIALIZER leinad_ui_t* leinad_instanciate_welem();

// defined inside every specific type header
    // LEINAD_FCLEANER void leinad_destroy_welem_instance();

/**
 * [GET] Gets a pointer to the data of a specific window element id
 *
 * @param id Identifier of the welem to get data from
 * @return pointer to the struct on success, NULL otherwise
 */
LEINAD_FGET leinad_welem_t* leinad_get_welem_data(leinad_welem_id_t id);

/**
 * [COMPARATOR] Compares 2 leinad_welem_instance_t
 *  1. LAYER
 *  2. X (lower side)
 *  3. Y (lower side)
 *
 * @param a First element to compare
 * @param b Second element to compare
 * @return `-1` if a < b, `0` if a == b, `1` if a > b
 */
LEINAD_FCOMPARATOR int leinad_compare_welem(const void* a, const void* b);
