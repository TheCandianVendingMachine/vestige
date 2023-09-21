#ifndef VESTIGE_BUTTON_H
#define VESTIGE_BUTTON_H

#include "aabb.h"
#include "ui/component.h"
#include "ui/ui_sprite.h"

typedef struct Button {
    Component component;
    UiSprite sprite;
    AABB click_bounds;
} Button;

#endif
