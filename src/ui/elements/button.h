#pragma once

#include <SDL3/SDL.h>

#include "../data.h"


/** 
 * Generic button data, to place/retrieve from `./elements.c -> welem_data[]`,
 *
 * @note This, represents the generic properties of one button, to use it on a UI you need
 *  to create an instance (`leinad_welem_button_instance_t`) of it. 
 * 
 * @see leinad_welem_button_instance_t
 */
typedef struct leinad_welem_button {
    int width, height;                  // base dimensions of the welem
    leinad_welem_type_t type;           // type of the welem
    leinad_welem_id_t id;               // id for retrieving/setting data to instances
                                            // these are specified inside `elements.h`
    struct leinad_clickable_data click; // clickable data
    Uint8 _data[LEINAD_WELEM_DATA_SIZE - sizeof(struct leinad_clickable_data)]; // unused
} leinad_welem_button_t;

/** 
 * Instance of a button to place on UIs.
 *
 * @note This, represents an instance of a welem, which can then be placed inside UIs,
 *  to define the data of a welem id: @see leinad_welem_t 
 * 
 * @see leinad_welem_t
 */
typedef struct leinad_welem_instance_button {
    leinad_welem_id_t id; // id for retrieving/setting data to instances
                                    // these are specified inside `elements.h`
    int x, y;                   // coordinates of the top-left corner INSIDE the UI
    leinad_welem_layer_t layer; // decides render/click order, relative to it's UI
    SDL_Texture* texture;       // texture to render the welem as, NULL for none
    
    Uint8 _data[LEINAD_WELEM_INSTANCE_DATA_SIZE]; // filler space to define extra behaviour
} leinad_welem_button_instance_t;



/** 
 * [INITIALIZER] Updates the data for a button, which must be freed when you dont need it
 *  anymore with `leinad_destroy_button()`
 * @param id identifier that this button will be assigned to, MUST BE LISTED ON `elements.h`
 * @param w width of the button in pixels
 * @param h height of the button in pixels
 * @param click_margin_x distance from both horizontal sides on which clicks dont get detected
 * @param click_margin_x distance from both vertical sides on which clicks dont get detected
 * @param action function to execute when the user clicks on the button
 * @return pointer to the created button data on success, NULL otherwise
 *
 * @note It is MANDATORY to call this on a specific welem_id in order for it to be usable,
 *  its behaviour is otherwise unsespecified
 */
LEINAD_FINITIALIZER leinad_welem_button_t* leinad_create_button(
    leinad_welem_id_t id,
    int w, int h,
    int click_margin_x, int click_margin_y,
    void (*action)(void*)
);

// /** 
//  * [CLEANER] THIS OPERATION IS NOT NEEDED AS THE DATA IS STATIC
//  * @param id Identifier of the button to release its data from
//  */
// LEINAD_FCLEANER void leinad_destroy_button(leinad_welem_button_t* button);


/** 
 * [INITIALIZER] Instanciates a button, which must be freed when you dont need it
 *  anymore with `leinad_destroy_button_instance()`
 * @param id identifier of the button that this instance refers to
 * @param x defines the left side of the ui
 * @param y defines the top side of the ui
 * @param layer decides render/click order, relative to it's UI
 * @param button_texture path to the desired texture, relative to `resources/ui/textures/`
 * @return pointer to the created button instance on success, NULL otherwise
 */
leinad_welem_button_instance_t* leinad_create_button_instance(
    leinad_welem_id_t id,
    leinad_ui_t* ui,
    int x, int y,
    leinad_welem_layer_t layer,
    const char* button_texture
);

/** 
 * [CLEANER] Frees the data of a button instance to release resources, making it no
 *  longer accesible
 * @param button pointer to the instance
 */
void leinad_destroy_button_instance(leinad_welem_button_instance_t* button);


