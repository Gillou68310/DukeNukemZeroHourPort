#include <stdbool.h>
#include <assert.h>
#include <ultra64.h>

#include "controller_api.h"
#include "../configfile.h"

#define MAXMAPPING 18
static int keyboard_buttons_down[MAXCONTROLLERS];
static int keyboard_mapping[MAXCONTROLLERS][MAXMAPPING][2];

static int keyboard_map_scancode(int cont, int scancode)
{
    int ret = 0;
    for (size_t i = 0; i < sizeof(keyboard_mapping[cont]) / sizeof(keyboard_mapping[cont][0]); i++)
    {
        if (keyboard_mapping[cont][i][0] == scancode)
        {
            ret |= keyboard_mapping[cont][i][1];
        }
    }
    return ret;
}

bool keyboard_on_key_down(int scancode)
{
    int mapped;
    for (s32 i = 0; i < MAXCONTROLLERS; i++)
    {
        mapped = keyboard_map_scancode(i, scancode);
        keyboard_buttons_down[i] |= mapped;
    }
    return true;
}

bool keyboard_on_key_up(int scancode)
{
    int mapped;
    for (s32 i = 0; i < MAXCONTROLLERS; i++)
    {
        mapped = keyboard_map_scancode(i, scancode);
        keyboard_buttons_down[i] &= ~mapped;
    }
    return true;
}

void keyboard_on_all_keys_up(void)
{
    for (s32 i = 0; i < MAXCONTROLLERS; i++)
    {
        keyboard_buttons_down[i] = 0;
    }
}

static void set_keyboard_mapping(int cont, int index, int mask, int scancode)
{
    assert(cont < MAXCONTROLLERS);
    assert(index < MAXMAPPING);
    keyboard_mapping[cont][index][0] = scancode;
    keyboard_mapping[cont][index][1] = mask;
}

static void keyboard_init(void)
{
    for (s32 i = 0; i < MAXCONTROLLERS; i++)
    {
        int j = 0;
        set_keyboard_mapping(i, j++, 0x80000, configKeyStickUp[i]);
        set_keyboard_mapping(i, j++, 0x10000, configKeyStickLeft[i]);
        set_keyboard_mapping(i, j++, 0x40000, configKeyStickDown[i]);
        set_keyboard_mapping(i, j++, 0x20000, configKeyStickRight[i]);
        set_keyboard_mapping(i, j++, U_JPAD, configKeyJoyUp[i]);
        set_keyboard_mapping(i, j++, L_JPAD, configKeyJoyLeft[i]);
        set_keyboard_mapping(i, j++, D_JPAD, configKeyJoyDown[i]);
        set_keyboard_mapping(i, j++, R_JPAD, configKeyJoyRight[i]);
        set_keyboard_mapping(i, j++, A_BUTTON, configKeyA[i]);
        set_keyboard_mapping(i, j++, B_BUTTON, configKeyB[i]);
        set_keyboard_mapping(i, j++, Z_TRIG, configKeyZ[i]);
        set_keyboard_mapping(i, j++, U_CBUTTONS, configKeyCUp[i]);
        set_keyboard_mapping(i, j++, L_CBUTTONS, configKeyCLeft[i]);
        set_keyboard_mapping(i, j++, D_CBUTTONS, configKeyCDown[i]);
        set_keyboard_mapping(i, j++, R_CBUTTONS, configKeyCRight[i]);
        set_keyboard_mapping(i, j++, R_TRIG, configKeyR[i]);
        set_keyboard_mapping(i, j++, L_TRIG, configKeyL[i]);
        set_keyboard_mapping(i, j++, START_BUTTON, configKeyStart[i]);
    }
}

static void keyboard_read(OSContPad *pad)
{
    for (s32 i = 0; i < MAXCONTROLLERS; i++)
    {
        pad[i].button |= keyboard_buttons_down[i];
        if ((keyboard_buttons_down[i] & 0x30000) == 0x10000)
        {
            pad[i].stick_x = -128;
        }
        if ((keyboard_buttons_down[i] & 0x30000) == 0x20000)
        {
            pad[i].stick_x = 127;
        }
        if ((keyboard_buttons_down[i] & 0xc0000) == 0x40000)
        {
            pad[i].stick_y = -128;
        }
        if ((keyboard_buttons_down[i] & 0xc0000) == 0x80000)
        {
            pad[i].stick_y = 127;
        }
    }
}

struct ControllerAPI controller_keyboard = {
    keyboard_init,
    keyboard_read
};
