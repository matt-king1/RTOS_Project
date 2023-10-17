/*
 * shield.h
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#ifndef SHIELD_SHIELD_H_
#define SHIELD_SHIELD_H_

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

#define SHIELD_ACTIVE_100MS 5
#define SHIELD_RECHARGE_100MS 15

#define PASSIVE_KINETIC_REDUCTION -0.75
#define ACTIVE_KINETIC_GAIN -1.25

struct ShieldState
{
  bool active;
  bool recharging;
};

void shieldTask(void);
void shieldTaskCreate(void);

#endif /* SHIELD_SHIELD_H_ */
