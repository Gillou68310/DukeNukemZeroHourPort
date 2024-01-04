#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ultra64.h>
#include <macros.h>

#include "fast3d/gfx_opengl.h"
#include "fast3d/gfx_sdl.h"
#include "fast3d/gfx_api.h"
#include "fast3d/gfx_window_manager_api.h"

#include "audio/audio_api.h"
#include "audio/audio_sdl.h"
#include "audio/audio_null.h"

#include "controller/controller_keyboard.h"

#include "configfile.h"

#define CONFIG_FILE "config.txt"

extern u8 *gCacheMemStart;
extern u8 *gCacheMemEnd;
extern Gfx *gDisplayList[];
extern volatile s64 D_800FE9E0;

void load_assets(void);
void mainLoop(void *arg);
void game_loop_one_iteration(void);

float __libm_qnan_f = 0x7F810000;
u32	osMemSize = 0x800000;
s32 osTvType = OS_TV_NTSC;
OSViMode osViModeTable[42];
static u8 gMemory[0x800000];

static struct AudioAPI *audio_api;
static struct GfxWindowManagerAPI *wm_api;
static struct GfxRenderingAPI *rendering_api;

static void save_config(void)
{
    configfile_save(CONFIG_FILE);
}

static void on_fullscreen_changed(bool is_now_fullscreen)
{
    configFullscreen = is_now_fullscreen;
}

int main(UNUSED int argc, UNUSED char *argv[])
{
    configfile_load(CONFIG_FILE);
    atexit(save_config);

    rendering_api = &gfx_opengl_api;
    wm_api = &gfx_sdl;

    gfx_current_native_viewport.width = 512;
    gfx_current_native_viewport.height = 384;
    gfx_framebuffers_enabled = false;
    gfx_msaa_level = 1;

    gfx_init(wm_api, rendering_api, "Duke Nukem Zero Hour", false, 512, 384, 100, 100);

    //wm_api->set_fullscreen_changed_callback(on_fullscreen_changed);
    wm_api->set_swap_interval(0);
    wm_api->set_target_fps(30);
    wm_api->set_keyboard_callbacks(keyboard_on_key_down, keyboard_on_key_up, keyboard_on_all_keys_up);
    rendering_api->set_texture_filter(FILTER_LINEAR);

    if (audio_api == NULL)
        audio_api = &audio_null;

    gCacheMemStart = &gMemory[0];
    gCacheMemEnd = &gMemory[0x800000];
    memset(gMemory, 0, sizeof(gMemory));

    load_assets();
    mainLoop(NULL);
    do
    {
        gfx_start_frame();
        game_loop_one_iteration();
        gfx_run(gDisplayList[0]);
        gfx_end_frame();
        D_800FE9E0 += 2;
    } while (1);

    return 0;
}

void func_80000000(s8 *arg0, s32 arg1)
{
}