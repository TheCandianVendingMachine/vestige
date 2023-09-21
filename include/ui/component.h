#ifndef VESTIGE_UI_COMPONENT_H
#define VESTIGE_UI_COMPONENT_H

#include "lib/math.h"
#include "ui/anchor.h"

typedef enum ComponentTypes {
    BUTTON
} ComponentTypes;

typedef struct Component {
    Vector2f position;
    Vector2f size;
    Anchor anchor;
} Component;

#endif
