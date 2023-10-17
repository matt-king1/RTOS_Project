/*
 * laser.h
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#ifndef LASER_LASER_H_
#define LASER_LASER_H_

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

#define STACK_SIZES 256u
#define TASK_PRIORITIES 21u

void laserTask(void);
void laserTaskCreate(void);
bool shootLaser(int ind);

#endif /* LASER_LASER_H_ */
