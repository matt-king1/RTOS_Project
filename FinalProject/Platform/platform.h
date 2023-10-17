/*
 * platform.h
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#ifndef PLATFORM_PLATFORM_H_
#define PLATFORM_PLATFORM_H_

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
#include "constants.h"

#define STACK_SIZES 256u
#define TASK_PRIORITIES 21u
#define TICK_TIME 5

// in pixels
#define PLATFORM_Y SCREEN_PIXELS - 15
#define PLATFORM_HEIGHT 5
#define PLATFORM_WIDTH 30

#define PLATFORM_MASS 10 // kg
#define MAX_PIXEL_FORCE 100 // kg * px/s^2
#define MAX_FORCE MAX_PIXEL_FORCE * SCREEN_MM / SCREEN_PIXELS //kg * mm/s^2
#define MAX_PIXEL_ACCEL MAX_PIXEL_FORCE / PLATFORM_MASS

#define PLATFORM_BOUNC_EN false
#define PLATFORM_BOUNCE_LIM false
#define MAX_BOUNCE_SPEED 0

struct PlatformData
{
  double x;
  double vx;
  double ax;
};


void platformTaskCreate(void);
void platformTask(void);
void platformTaskCreate(void);


#endif /* PLATFORM_PLATFORM_H_ */
