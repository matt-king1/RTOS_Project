/*
 * engine.h
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#ifndef PLATFORM_ENGINE_H_
#define PLATFORM_ENGINE_H_

#include <stdbool.h>
#include <gpio.h>
#include <capsense.h>
#include "os.h"
#include "sl_board_control.h"
#include "em_assert.h"
#include "em_emu.h"
#include "glib.h"
#include "dmd.h"
#include "os_trace.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "HM/hm.h"
#include "Platform/platform.h"
#include "Shield/shield.h"
#include "GameManager/gameManager.h"
#include "constants.h"
#include "Laser/laser.h"

#define STACK_SIZES 256u
#define TASK_PRIORITIES 21u

#define GRAVITY GRAVITY_PIXELS * SCREEN_MM / SCREEN_PIXELS
#define GAME_OVER_SPEED 25
#define PHYSICS_PERIOD 1
#define PHYSICS_DELTA (PHYSICS_PERIOD / 10.0)

void engineTask(void);
void engineTaskCreate(void);

void updateHMs(struct HoltzmanData HMs_[]);
void updatePlatform(struct PlatformData *platData_);
void checkHMs(struct HoltzmanData HMs_[], struct PlatformData *platData_, struct ShieldState *shieldState_);


#endif /* PLATFORM_ENGINE_H_ */
