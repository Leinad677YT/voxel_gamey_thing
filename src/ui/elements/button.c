#pragma once

#include <SDL3/SDL.h>

#include "../../data/globals.h"

#include "button.h"
#include "../rendering.h"

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
) {
    welem_data[id].id = id;
    welem_data[id].type = LEINAD_WELEM_BUTTON;
    welem_data[id].width = w;
    welem_data[id].height = h;
    ((leinad_welem_button_t*) (&welem_data[id]))->click.click_margin_x = click_margin_x;
    ((leinad_welem_button_t*) (&welem_data[id]))->click.click_margin_y = click_margin_y;
    ((leinad_welem_button_t*) (&welem_data[id]))->click.action = action;

    return (leinad_welem_button_t*)&welem_data[id];
}

// /** 
//  * [CLEANER] THIS OPERATION IS NOT NEEDED AS THE DATA IS STATIC
//  * @param id Identifier of the button to release its data from
//  */
// LEINAD_FCLEANER void leinad_destroy_button(leinad_welem_button_t* button);


/** 
 * [INITIALIZER] Instanciates a button, which must be freed when you dont need it
 *  anymore with `leinad_destroy_button_instance()`
 * @param id identifier of the button that this instance refers to
 * @param ui identifier of the UI the button will be in
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
) {
    leinad_welem_button_instance_t* button;

    button = SDL_malloc(sizeof(struct leinad_welem_instance));

    if (button == NULL) return NULL;

    button->texture = leinad_load_texture(ui->renderer,button_texture);
    button->id = id;
    button->layer = layer;
    button->x = x;
    button->y = y;

    if (!leinad_insert_slist(&ui->welems,button)){
        SDL_free(button->texture);
        SDL_free(button);
        return NULL;
    }

    return button;
}

/** 
 * [CLEANER] Frees the data of a button instance to release resources, making it no
 *  longer accesible
 * @param button pointer to the instance
 */
void leinad_destroy_button_instance(leinad_welem_button_instance_t* button){
    if (button == NULL) return;
    SDL_free(button->texture);
    SDL_free(button);
}
