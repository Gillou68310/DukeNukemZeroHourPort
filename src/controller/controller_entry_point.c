#include <PR/os.h>
#include "macros.h"

#include "controller_recorded_tas.h"
#include "controller_keyboard.h"
#include "controller_sdl.h"

static struct ControllerAPI *controller_implementations[] = {
    &controller_recorded_tas,
    &controller_sdl,
    &controller_keyboard,
};

s32 osContInit(UNUSED OSMesgQueue *mq, u8 *controllerBits, UNUSED OSContStatus *status)
{
    /*TODO: other controllers?*/
    status[0].type = 5;
    status[0].status = 1;
    status[0].errno = 0;

    for (size_t i = 0; i < sizeof(controller_implementations) / sizeof(struct ControllerAPI *); i++)
    {
        controller_implementations[i]->init();
    }
    *controllerBits = 1;
    return 0;
}

s32 osContStartReadData(UNUSED OSMesgQueue *mesg)
{
    return 0;
}

void osContGetReadData(OSContPad *pad)
{
    pad->button = 0;
    pad->stick_x = 0;
    pad->stick_y = 0;
    pad->errno = 0;

    for (size_t i = 0; i < sizeof(controller_implementations) / sizeof(struct ControllerAPI *); i++)
    {
        controller_implementations[i]->read(pad);
    }
}
