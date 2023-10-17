/*
 * laser.c
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */
#include "laser.h"
#include "HM/hm.h"


static OS_TCB laserTCB;
static CPU_STK laserSTK[STACK_SIZES];

OS_SEM LaserSem;

extern OS_MUTEX HMMutex;
extern uint8_t HM_COUNT;
extern struct HoltzmanData HMs[];
extern int score;

uint8_t laserCharges = 5;
bool autoCannon = true;
int lasersFired = 0;
int shotX;
int shotY;

void laserTask(void);

void laserTaskCreate (void) {
    RTOS_ERR     semErr, tskErr;

    OSSemCreate(&LaserSem,
                "Laser Semaphore",
                0,
                &semErr);

    OSTaskCreate(&laserTCB,                /* Pointer to the task's TCB.  */
                 "Laser Task",                    /* Name to help debugging.     */
                 (OS_TASK_PTR)&laserTask,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  NORMAL_PRIO,             /* Task's priority.            */
                 &laserSTK[0],             /* Pointer to base of stack.   */
                 (STACK_SIZES / 10u),  /* Stack limit, from base.     */
                  STACK_SIZES,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  120u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &tskErr);

    if (semErr.Code || tskErr.Code) EFM_ASSERT(false);
}

bool shootLaser(int ind)
{
  if(laserCharges > 0)
  {
      laserCharges--;
      score++;
      lasersFired = 3;
      shotX = HMs[ind].x;
      shotY = HMs[ind].y;

      RTOS_ERR mutErr;
      OSMutexPend(&HMMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      generateHM(ind);

      OSMutexPost(&HMMutex, OS_OPT_POST_NONE, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      return true;
  }
  return false;
}

void laserTask(void)
{
  RTOS_ERR semErr;

  while(1)
  {
      OSSemPend(&LaserSem, 0, OS_OPT_PEND_BLOCKING, NULL, &semErr);
      if(semErr.Code) EFM_ASSERT(false);

      int highestInd = 0;
      int highestY = 0;

      for(int i = 0; i < HM_COUNT; i++)
      {
          if(HMs[i].y > highestY)
          {
              highestY = HMs[i].y;
              highestInd = i;
          }
      }

      shootLaser(highestInd);
  }
}

