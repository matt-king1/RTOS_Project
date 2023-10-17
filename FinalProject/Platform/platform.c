/*
 * platform.c
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */
#include "platform.h"

#define PLATFORM_PERIOD 1

int PLATFORM_BOUNCE_ENABLED;
int MAX_SPEED = 100;

OS_MUTEX PlatformMutex;
OS_TMR PlatformTimer;

static OS_SEM PlatformSem;
static OS_TCB platformTCB;
static CPU_STK platformSTK[STACK_SIZES];

struct PlatformData platformData;

void platformTask(void);

void platformTimerCB(void)
{
  RTOS_ERR semErr;
  OSSemPost(&PlatformSem, OS_OPT_POST_1, &semErr);
  if(semErr.Code) EFM_ASSERT(false);
}

void platformTaskCreate(void)
{
  RTOS_ERR semErr, tmrErr, tskErr, mutErr;

  OSSemCreate(&PlatformSem,
              "Platform Semaphore",
              0,
              &semErr);

  OSMutexCreate(&PlatformMutex,
                "Platform Mutex",
                &mutErr);

  OSTmrCreate(&PlatformTimer,
              "Platform Timer",
              0,
              PLATFORM_PERIOD,
              OS_OPT_TMR_PERIODIC,
              (OS_TMR_CALLBACK_PTR)&platformTimerCB,
              NULL,
              &tmrErr);

  OSTaskCreate(&platformTCB,                /* Pointer to the task's TCB.  */
               "Platform Task",                    /* Name to help debugging.     */
               (OS_TASK_PTR)&platformTask,                   /* Pointer to the task's code. */
               DEF_NULL,                          /* Pointer to task's argument. */
               HIGHER_PRIO-1,             /* Task's priority.            */
               &platformSTK[0],             /* Pointer to base of stack.   */
               (STACK_SIZES / 10u),  /* Stack limit, from base.     */
               STACK_SIZES,         /* Stack size, in CPU_STK.     */
               10u,                               /* Messages in task queue.     */
               120u,                                /* Round-Robin time quanta.    */
               DEF_NULL,                          /* External TCB data.          */
               OS_OPT_TASK_STK_CHK,               /* Task options.               */
               &tskErr);

  if(semErr.Code || mutErr.Code || tmrErr.Code || tskErr.Code)
  {
      EFM_ASSERT(false);
  }

  PLATFORM_BOUNCE_ENABLED = false;
  platformData.ax = 0;
  platformData.vx = 0;
  platformData.x = SCREEN_PIXELS/2;
}

void platformTask(void)
{
  RTOS_ERR semErr;
  RTOS_ERR mutErr;

  while(1)
  {
      OSSemPend(&PlatformSem, 0, OS_OPT_PEND_BLOCKING, NULL, &semErr);
      if(semErr.Code) EFM_ASSERT(false);

      CAPSENSE_Sense();
      int pressed = -1;
      for(int i = 0; i < 4; i++)
      {
          if(CAPSENSE_getPressed(i))
          {
              if(pressed == -1) pressed = i;
              else {
                  pressed = -1;
                  break;
              }
          }
      }

      OSMutexPend(&PlatformMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      switch(pressed) {
        case 0:
          platformData.ax = -24;
          break;
        case 1:
          platformData.ax = -12;
          break;
        case 2:
          platformData.ax = 12;
          break;
        case 3:
          platformData.ax = 24;
          break;
        default:
          platformData.ax = 0;
          break;
      }

      OSMutexPost(&PlatformMutex, OS_OPT_POST_NONE, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);
  }
}
