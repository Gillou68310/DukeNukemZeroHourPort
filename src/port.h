#ifndef PORT_H
#define PORT_H

#include "fast3d/gfx_api.h"
#include "configfile.h"

void audio_task(void);

#define ASPECT_RATIO (gfx_current_window_dimensions.aspect_ratio)
#define BORDER_SIZE (0)

#endif