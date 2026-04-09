#include <SDL3/SDL.h>

#include "../data/globals.h"

#include "data.h"
#include "rendering.h"

#include "elements/button.h"



// [[[ 02. UI ]]]



/** 
 * [BUILDER] Creates the data for a ui, which must be freed when you dont need it
 *  anymore with `leinad_destroy_ui()`
 * A ui contains window elements, which can contain a visual representation and
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
LEINAD_FBUILDER leinad_ui_t* leinad_create_ui(int w, int h, bool kidnaps_screen) {

    leinad_ui_t* ui;

    ui = SDL_malloc(sizeof(struct leinad_ui));

    if (ui == NULL) return NULL;

    if (!leinad_create_slist(&ui->welems, leinad_compare_welem)) {
        SDL_free(ui);
        return NULL;
    }
    
    ui->surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA8888);

    if (ui->surface == NULL) {
        SDL_free(ui->welems);
        SDL_free(ui);
        return NULL;
    }

    ui->renderer = SDL_CreateSoftwareRenderer(ui->surface);
     
    if (ui->renderer == NULL) {
        SDL_DestroySurface(ui->surface);
        SDL_free(ui->welems);
        SDL_free(ui);
        return NULL;
    }


    ui->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,w,h);

    if (ui->texture == NULL) {
        SDL_DestroySurface(ui->surface);
        SDL_DestroyRenderer(ui->renderer);
        SDL_free(ui->welems);
        SDL_free(ui);
        return NULL;
    }


    ui->kidnaps_screen = kidnaps_screen;
    ui->w = w;
    ui->h = h;

    return ui;
}


LEINAD_FITERATOR void* aux_removal_iterator(void* acc, void* node) {
    leinad_welem_instance_t* inst;

    leinad_welem_t* data;
    
    inst = leinad_getelement_slist(*(leinad_slist_node_t*)node);

    data = leinad_get_welem_data(inst->id);

    switch(data->type){
        case LEINAD_WELEM_BUTTON:
            leinad_destroy_button_instance((leinad_welem_button_instance_t*) inst);
    }
     
    return acc;
}

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
LEINAD_FCLEANER void leinad_destroy_ui(leinad_ui_t* ui) {

    if (ui == NULL) return;

    leinad_forall_slist(leinad_getfirst_slist(ui->welems), aux_removal_iterator, &leinad_destroy_ui, false);
    leinad_destroy_slist(&ui->welems);
    SDL_DestroyTexture(ui->texture);
    SDL_DestroyRenderer(ui->renderer);
    SDL_DestroySurface(ui->surface);

    SDL_free(ui);

    return;
}


LEINAD_AUX struct aux_render_welem_iterator {
    SDL_Renderer* renderer;
    SDL_Surface* surface;
    SDL_Surface* aux_surface;
    bool ret;
};

LEINAD_AUX LEINAD_FITERATOR static void* render_welem_iterator(void* accumulator, void* node) {
    
    leinad_welem_instance_t* current;
    leinad_welem_t* current_data;
    
    if (node == NULL) goto ret;

    current = leinad_getelement_slist(*(leinad_slist_node_t*)node);

    
    if (current == NULL) goto ret;

    current_data = leinad_get_welem_data(current->id);


    switch (current_data->type){
        case LEINAD_WELEM_BUTTON:
        case LEINAD_WELEM_TEXTURE:
                leinad_render_9crop(current, ((struct aux_render_welem_iterator*)(accumulator))->renderer);
            break;

        default: // skip for unknowns
            break;
        
    }


ret:
    return accumulator;
}

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
bool leinad_update_ui(leinad_ui_t* ui) {

    struct aux_render_welem_iterator data = {
        .renderer = ui->renderer,
        .surface = ui->surface,
        .aux_surface = NULL,
        .ret = true
    };




    if (!SDL_LockTextureToSurface(ui->texture, NULL, &data.aux_surface)) {
        SDL_Log("Couldn't update ui: %s",SDL_GetError());
        data.ret = false;
        return data.ret;
    }

    // transparent background (its a UI layer afterall)
    SDL_SetRenderDrawColor(data.renderer, 0x00, 0x00,0x00, SDL_ALPHA_TRANSPARENT);
    SDL_RenderClear(data.renderer);


    leinad_slist_node_t* start = leinad_getfirst_slist(ui->welems);

    // create surface
    leinad_forall_slist(start, render_welem_iterator, &data, false);


    SDL_RenderPresent(data.renderer);

    SDL_BlitSurface(ui->surface,NULL,data.aux_surface,NULL);

    SDL_UnlockTexture(ui->texture);

    SDL_DestroySurface(data.aux_surface);

    return data.ret;
}


LEINAD_AUX struct aux_render_ui_iterator {
    bool ret;
};

#define ZAUX_ELEMENT ((leinad_ui_instance_t*)element)

LEINAD_AUX LEINAD_FITERATOR static void* render_ui_iterator(void* accumulator, void* element) {
    
    SDL_FRect dst = {
        .h = ZAUX_ELEMENT->ui->h * ZAUX_ELEMENT->scale,
        .w = ZAUX_ELEMENT->ui->w * ZAUX_ELEMENT->scale,
        .x = ZAUX_ELEMENT->x,
        .y = ZAUX_ELEMENT->y
    };


    if ( // nothing to draw
        element == NULL
        || ZAUX_ELEMENT->ui->texture == NULL
    ) goto ret;


    if (!SDL_RenderTexture(renderer,ZAUX_ELEMENT->ui->texture,NULL,&dst)){
        ((struct aux_render_ui_iterator*)accumulator)->ret = false;
        SDL_LogError(SDL_LOG_CATEGORY_RENDER,"Couldn't blit UI layer onto the screen: %s",SDL_GetError());
    }

ret:
    return accumulator;
}

/** 
 * [CALL] Renders all UI layers over the window
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 */
LEINAD_FCALL bool leinad_render_ui() {

    struct aux_render_ui_iterator data = {
        .ret = true
    };

    // loop is reversed to render deeper layers first
    leinad_forall_stack(&ui_render_stack, render_ui_iterator, &data, true);

    return data.ret;
}

/** 
 * [INITIALIZER] Makes a UI instance that can be later off freed by calling
 *  `leinad_destroy_ui_instance()` on it when it's no longer needed
 *
 * @param ui pointer to the UI you want to render   
 * @param x defines the left side of the ui
 * @param y defines the top side of the ui
 * @param scale Scale of the UI, must be at least 1
 * @return Pointer to the instance on success, NULL otherwise
 */
LEINAD_FINITIALIZER leinad_ui_instance_t* leinad_instanciate_ui(leinad_ui_t* ui, int x, int y, Uint8 scale) {

    leinad_ui_instance_t* instance;

    instance = SDL_malloc(sizeof(leinad_ui_instance_t));

    if (instance == NULL) return NULL;

    instance->ui = ui;
    instance->x = x;
    instance->y = y;
    instance->scale = scale;    

    return instance;
}

/** 
 * [CLEANER] Frees the data of a UI instance to release resources, making it no
 *  longer accesible
 * @param ui pointer to the instance
 */
LEINAD_FCLEANER void leinad_destroy_ui_instance(leinad_ui_instance_t* ui) {

    SDL_free(ui);
}


/** 
 * [CALL] Marks a UI to be the one active, enabling it's screen interactions
 * @param ui pointer to the UI you want to render, must be properly instanciated 
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 *
 * @note A UI does not need to be rendered to be active, and neither is the other
 *  way around.
 */
LEINAD_FCALL bool leinad_set_active_ui(leinad_ui_instance_t* ui) {

    ui_active = ui;

    return true;
}

/** 
 * Pushes a UI to the "rendering stack", placing it over the other ones
 * @param ui pointer to the UI you want to render, must be properly instanciated 
 * @param x defines the left side of the ui
 * @param y defines the top side of the ui
 * @param scale Scale of the target ui, must be at least 1
 * @return `true` on success, `false` otherwise, logs errors to the SDL log when
 *  they happen
 *
 * @note A UI does not need to be rendered to be active, and neither is the other
 *  way around.
 */
bool leinad_push_ui(leinad_ui_instance_t* ui) {
    return leinad_push_stack(&ui_render_stack, ui);
}

struct aux_check_click_iterator {
    SDL_Event* event;
    Uint8 ret;
};

LEINAD_AUX LEINAD_FITERATOR static void* check_click_iterator(void* accumulator, void* node) {
    leinad_welem_instance_t* welem = leinad_getelement_slist(*(leinad_slist_node_t*)node);
    leinad_welem_t* welem_data;

    if (welem == NULL) goto ret;

    welem_data = leinad_get_welem_data(welem->id);

    if (welem_data == NULL) goto ret;

    switch(welem_data->type) {
        case LEINAD_WELEM_BUTTON:
            if ( // not left click
                ((struct aux_check_click_iterator*)(accumulator))->event->button.type != SDL_EVENT_MOUSE_BUTTON_DOWN
                || ((struct aux_check_click_iterator*)(accumulator))->event->button.button != SDL_BUTTON_LEFT
            ) break;

            if ( // in range
                ((struct aux_check_click_iterator*)(accumulator))->event->button.x >= 
                    ui_active->x +(welem->x + ((leinad_welem_button_t*)welem_data)->click.click_margin_x) *ui_active->scale
                && ((struct aux_check_click_iterator*)(accumulator))->event->button.x <= 
                    ui_active->x +(welem->x + welem_data->width - ((leinad_welem_button_t*)welem_data)->click.click_margin_x) *ui_active->scale
                && ((struct aux_check_click_iterator*)(accumulator))->event->button.y >= 
                    ui_active->y +(welem->y + ((leinad_welem_button_t*)welem_data)->click.click_margin_y) *ui_active->scale
                && ((struct aux_check_click_iterator*)(accumulator))->event->button.y <= 
                    ui_active->y +(welem->y + welem_data->height - ((leinad_welem_button_t*)welem_data)->click.click_margin_y) *ui_active->scale
            ) {
                SDL_Log("test");
            }

        default: break;
    }

ret:
    return accumulator;
}

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
LEINAD_FCALL int manage_ui_interaction(void *appstate, SDL_Event* event) {

    struct aux_check_click_iterator click; 
    
    click.event = event;
    click.ret = LEINAD_UI_STATUS_CONTINUE;

    leinad_forall_slist(leinad_getfirst_slist(ui_active->ui->welems), check_click_iterator, &click, false);

    return click.ret;
}





// [[[ 03. WELEM ]]]



// defined inside every specific type header
    // LEINAD_FBUILDER leinad_ui_t* leinad_create_ui(int w, int h, bool kidnaps_screen);

// defined inside every specific type header
    // LEINAD_FCLEANER void leinad_destroy_ui(leinad_ui_t* ui);

/**
 * [GET] Gets a pointer to the data of a specific window element id
 *
 * @param id Identifier of the welem to get data from
 * @return pointer to the struct on success, NULL otherwise
 */
LEINAD_FGET leinad_welem_t* leinad_get_welem_data(leinad_welem_id_t id) {

    return &welem_data[id];
}

#define ZCAST(var) ((leinad_welem_instance_t*)var)

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
LEINAD_FCOMPARATOR int leinad_compare_welem(const void* a, const void* b) {

    if (ZCAST(a)->layer < ZCAST(b)->layer) return -1;
    if (ZCAST(a)->layer > ZCAST(b)->layer) return 1;

    if (ZCAST(a)->x < ZCAST(b)->x) return -1;
    if (ZCAST(a)->x > ZCAST(b)->x) return 1;

    if (ZCAST(a)->y < ZCAST(b)->y) return -1;
    if (ZCAST(a)->y > ZCAST(b)->y) return 1;

    return 0;
}

#include "elements.c"
#include "rendering.c"

#include "elements/button.c"

#include "screens.c"