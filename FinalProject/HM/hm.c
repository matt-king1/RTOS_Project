/*
 * hm.c
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#include "hm.h"
#include <stdlib.h>

uint8_t HM_COUNT = 1;
OS_MUTEX HMMutex;
struct HoltzmanData HMs[3];

void hm_init(void)
{
  RTOS_ERR mutErr;

  OSMutexCreate(&HMMutex,
              "HM Mutex",
              &mutErr);

  if(mutErr.Code) EFM_ASSERT(false);

  for(int i = 0; i < HM_COUNT; i++) generateHM(i);
}

void generateHM(int idx)
{
  HMs[idx] = (struct HoltzmanData){
      .x = (rand() % (CANYON_SIZE_PIXELS - (2 * HM_PIXEL_RADIUS))) + HM_PIXEL_RADIUS + CANYON_START,
      .y = HM_PIXEL_RADIUS,
      .vx = (rand() % 40) - 20,
      .vy = rand() % 10,
      .mode = 0
  };
}
