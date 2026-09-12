#include <string.h>
#include <stdlib.h>
#include "sm64.h"
#include "types.h"
#include "game/area.h"
#include "game/camera.h"
#include "game/interaction.h"
#include "game/level_update.h"
#include "game/ingame_menu.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/save_file.h"
#include "game/sound_init.h"
#include "game/spawn_object.h"
#include "game/moving_texture.h"
#include "audio/external.h"
#include "engine/math_util.h"
#include "behavior_data.h"

/*
 * Platform/sandbox services that do not exist in the single-platform demo.
 * The gameplay action files themselves are the unmodified SM64 sources.
 */

s16 gCurrAreaIndex = 1;
struct CreditsEntry *gCurrCreditsEntry = NULL;
struct HudDisplay gHudDisplay = {0};
u8 gLastCompletedCourseNum = COURSE_NONE;
s8 gNeverEnteredCastle = FALSE;
f32 gPaintingMarioYEntry = 0.0f;
s16 gSaveOptSelectIndex = MENU_OPT_NONE;

static struct Object *sSandboxObject = NULL;
struct Object *gCurrentObject = NULL;

const BehaviorScript bhvBowserKeyCourseExit[] = {0};
const BehaviorScript bhvBowserKeyUnlockDoor[] = {0};
const BehaviorScript bhvCelebrationStar[] = {0};
const BehaviorScript bhvEndPeach[] = {0};
const BehaviorScript bhvEndToad[] = {0};
const BehaviorScript bhvGiantPole[] = {0};
const BehaviorScript bhvKoopaShellUnderwater[] = {0};
const BehaviorScript bhvSparkleSpawn[] = {0};
const BehaviorScript bhvStaticObject[] = {0};
const BehaviorScript bhvTree[] = {0};
const BehaviorScript bhvUnlockDoorStar[] = {0};

void *virtual_to_segmented(u32 segment, const void *addr) {
    (void) segment;
    return (void *) addr;
}

void create_dialog_box_with_var(s16 dialog, s32 dialogVar) {
    (void) dialog;
    (void) dialogVar;
}

void create_dialog_inverted_box(s16 dialog) {
    (void) dialog;
}

void disable_background_sound(void) {}
void enable_background_sound(void) {}
void disable_time_stop(void) {}
void enable_time_stop(void) {}

void fade_into_special_warp(u32 arg, u32 color) {
    (void) arg;
    (void) color;
}

u32 get_door_save_file_flag(struct Object *door) {
    (void) door;
    return 0;
}

struct Object *mario_get_collided_object(struct MarioState *m, u32 interactType) {
    (void) m;
    (void) interactType;
    return NULL;
}

/* Exact tiny helper logic from interaction.c, kept here to avoid linking the
 * whole object-interaction system in the empty sandbox. */
s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *o) {
    f32 dx = o->oPosX - m->pos[0];
    f32 dz = o->oPosZ - m->pos[2];
    return atan2s(dz, dx);
}

void obj_mark_for_deletion(struct Object *obj) {
    if(obj != NULL) obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}

void obj_scale(struct Object *obj, f32 scale) {
    if(obj == NULL) return;
    obj->header.gfx.scale[0] = scale;
    obj->header.gfx.scale[1] = scale;
    obj->header.gfx.scale[2] = scale;
}

void override_viewport_and_clip(Vp *a, Vp *b, u8 c, u8 d, u8 e) {
    (void) a; (void) b; (void) c; (void) d; (void) e;
}

void play_course_clear(void) {}
void play_cutscene_music(u16 seqArgs) { (void) seqArgs; }
void play_peachs_jingle(void) {}
void play_shell_music(void) {}
void stop_shell_music(void) {}

void play_transition(s16 transType, s16 time, u8 red, u8 green, u8 blue) {
    (void) transType; (void) time; (void) red; (void) green; (void) blue;
}

void reset_cutscene_msg_fade(void) {}

static u32 sSaveFlags = 0;
void save_file_set_flags(u32 flags) { sSaveFlags |= flags; }
void save_file_clear_flags(u32 flags) { sSaveFlags &= ~flags; }

void set_cutscene_message(s16 xOffset, s16 yOffset, s16 msgIndex, s16 msgDuration) {
    (void) xOffset; (void) yOffset; (void) msgIndex; (void) msgDuration;
}

void set_menu_mode(s16 mode) { (void) mode; }
void sound_banks_enable(u8 player, u16 bankMask) { (void) player; (void) bankMask; }

static struct Object *sandbox_spawn(struct Object *parent, const BehaviorScript *behavior) {
    /*
     * The empty test platform normally never spawns helper objects. Keeping a
     * full struct Object permanently in .bss cost roughly 1 KiB of the very
     * small Graph 35+E II RAM budget, so allocate this one-object sandbox pool
     * lazily only if a cutscene/action actually requests a spawn.
     */
    if(sSandboxObject == NULL) {
        sSandboxObject = calloc(1, sizeof(*sSandboxObject));
        if(sSandboxObject == NULL) return NULL;
    }
    else {
        memset(sSandboxObject, 0, sizeof(*sSandboxObject));
    }

    sSandboxObject->parentObj = parent;
    sSandboxObject->behavior = behavior;
    sSandboxObject->activeFlags = ACTIVE_FLAG_ACTIVE;
    sSandboxObject->header.gfx.scale[0] = 1.0f;
    sSandboxObject->header.gfx.scale[1] = 1.0f;
    sSandboxObject->header.gfx.scale[2] = 1.0f;
    gCurrentObject = sSandboxObject;
    return sSandboxObject;
}

struct Object *spawn_object(struct Object *parent, s32 model, const BehaviorScript *behavior) {
    (void) model;
    return sandbox_spawn(parent, behavior);
}

struct Object *spawn_object_abs_with_rot(struct Object *parent, s16 uselessArg, u32 model,
        const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 pitch, s16 yaw, s16 roll) {
    (void) uselessArg; (void) model;
    struct Object *o = sandbox_spawn(parent, behavior);
    o->oPosX = x; o->oPosY = y; o->oPosZ = z;
    o->oFaceAnglePitch = pitch; o->oFaceAngleYaw = yaw; o->oFaceAngleRoll = roll;
    return o;
}
