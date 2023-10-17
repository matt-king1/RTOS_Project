/*
 * hm.h
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#ifndef HM_HM_H_
#define HM_HM_H_

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

#define HM_PIXEL_RADIUS 3
#define HOLTZMAN_DIAMETER (2*HM_PIXEL_RADIUS) * SCREEN_MM / SCREEN_PIXELS
#define SIDE_KINETIC_REDUCTION

struct HoltzmanData
{
  double x;
  double y;
  double vx;
  double vy;
  uint8_t mode;
};

void hm_init(void);
void generateHM(int idx);

#endif /* HM_HM_H_ */
