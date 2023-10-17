/*
 * led.h
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#ifndef LED_LED_H_
#define LED_LED_H_

#include <gpio.h>
#include "os.h"
#include "sl_board_control.h"
#include "em_assert.h"
#include "em_emu.h"
#include "glib.h"
#include "dmd.h"
#include "os_trace.h"
#include "stdio.h"
#include "string.h"
#include "Platform/platform.h"
#include "Shield/shield.h"
#include "GameManager/gameManager.h"

#define STACK_SIZES 256u
#define TASK_PRIORITIES 21u

void ledTask(void);
void ledTaskCreate(void);

#endif /* LED_LED_H_ */
