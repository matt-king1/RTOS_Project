/*
 * led.c
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */


#include "led.h"
#include <math.h>

extern enum gameState_e gameState;
extern struct PlatformData platformData;
extern int MAX_SPEED;

static OS_TCB ledTCB;
static CPU_STK ledSTK[STACK_SIZES];
static OS_SEM LEDSem;

OS_TMR LEDTimers[3];

void ledTimerCB(void)
{
  RTOS_ERR semErr;
  if(gameState == INPROGRESS)
  {
      OSSemPost(&LEDSem, OS_OPT_POST_1, &semErr);
      if(semErr.Code) EFM_ASSERT(false);

      toggleLED();
  }
}

void ledTask(void);

void ledTaskCreate (void) {
    RTOS_ERR     semErr, tmrErr0, tmrErr1, tmrErr2, tskErr;

    OSSemCreate(&LEDSem,
                "LED Semaphore",
                0,
                &semErr);

    OSTmrCreate(&LEDTimers[0],
                "LED Timer 0",
                0,
                10,
                OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR)&ledTimerCB,
                NULL,
                &tmrErr0);

    OSTmrCreate(&LEDTimers[1],
                "LED Timer 1",
                0,
                5,
                OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR)&ledTimerCB,
                NULL,
                &tmrErr1);

    OSTmrCreate(&LEDTimers[2],
                "LED Timer 2",
                0,
                1,
                OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR)&ledTimerCB,
                NULL,
                &tmrErr2);

    OSTaskCreate(&ledTCB,                /* Pointer to the task's TCB.  */
                 "LED Task",                    /* Name to help debugging.     */
                 (OS_TASK_PTR)&ledTask,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  NORMAL_PRIO,             /* Task's priority.            */
                 &ledSTK[0],             /* Pointer to base of stack.   */
                 (STACK_SIZES / 10u),  /* Stack limit, from base.     */
                  STACK_SIZES,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  120u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &tskErr);

    if(semErr.Code || tmrErr0.Code || tmrErr1.Code || tmrErr2.Code || tskErr.Code) EFM_ASSERT(false);
}

void ledTask(void)
{
  RTOS_ERR semErr, tmrErr;

  while(1)
  {
    OSSemPend(&LEDSem, 0, OS_OPT_PEND_BLOCKING, NULL, &semErr);
    if(semErr.Code) EFM_ASSERT(false);

    int ledPercent = fmin(2, round((fabs(platformData.vx) / MAX_SPEED) * 2.0));

    for(int i = 0; i < 3; i++)
    {
      if(ledPercent == i)
      {
        if(OSTmrStateGet(&LEDTimers[i], &tmrErr) != OS_TMR_STATE_RUNNING) OSTmrStart(&LEDTimers[i], &tmrErr);
        if(tmrErr.Code) EFM_ASSERT(false);
      } else
      {
        if(OSTmrStateGet(&LEDTimers[i], &tmrErr) != OS_TMR_STATE_STOPPED) OSTmrStop(&LEDTimers[i], OS_OPT_TMR_NONE, NULL, &tmrErr);
        if(tmrErr.Code) EFM_ASSERT(false);
      }
    }
  }
}

