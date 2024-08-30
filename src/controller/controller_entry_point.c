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
    *controllerBits = 0;
    for (s32 i = 0; i < MAXCONTROLLERS; i++)
    {
      status[i].type = CONT_TYPE_NORMAL;
      status[i].status = 1;
      status[i].errno = 0;
      *controllerBits |= (1<<i);
    }

    for (size_t i = 0; i < sizeof(controller_implementations) / sizeof(struct ControllerAPI *); i++)
    {
        controller_implementations[i]->init();
    }

    return 0;
}

s32 osContStartReadData(UNUSED OSMesgQueue *mesg)
{
    return 0;
}

void osContGetReadData(OSContPad *pad)
{
    for (s32 i = 0; i < MAXCONTROLLERS; i++)
    {
        pad[i].button = 0;
        pad[i].stick_x = 0;
        pad[i].stick_y = 0;
        pad[i].errno = 0;
    }

    for (size_t i = 0; i < sizeof(controller_implementations) / sizeof(struct ControllerAPI *); i++)
    {
        controller_implementations[i]->read(pad);
    }
}
