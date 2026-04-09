#include <SDL3/SDL.h>

#include "../data/tags.h"
#include "../data/app.h"

#include "data.h"
#include "elements/button.h"

#include <stdio.h>

LEINAD_FDEFINITION leinad_ui_t* uiStart_load(){
    leinad_ui_t* ui = leinad_create_ui(LEINAD_WINDOW_WIDTH, LEINAD_WINDOW_HEIGHT, true);

    if (ui == NULL) return NULL;

    leinad_welem_button_instance_t* yippiee = leinad_create_button_instance(
        LEINAD_WELEMD_YIPIEE_BUTTON, ui,
        0, 0, LEINAD_WELEM_LAYER1,
         "button_03"
    );

    leinad_welem_button_instance_t* yippiee2 = leinad_create_button_instance(
        LEINAD_WELEMD_YIPIEE_BUTTON, ui,
        40, 80, LEINAD_WELEM_LAYER1,
         "button_02"
    );


    leinad_welem_button_instance_t* yippiee3 = leinad_create_button_instance(
        LEINAD_WELEMD_YIPIEE_BUTTON, ui,
        20, 30, LEINAD_WELEM_LAYER1,
        "button_01"
    );

    if (yippiee == NULL) {
        leinad_destroy_ui(ui);
        return NULL;
    }

    leinad_welem_button_instance_t *a =leinad_getelement_slist(*leinad_getfirst_slist(ui->welems));

    if (!leinad_update_ui(ui)){
        leinad_destroy_button_instance(yippiee);
        leinad_destroy_ui(ui);
        return NULL;
    }

    return ui;
}