/*
 * shield.c
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */


#include "shield.h"

OS_Q ShieldMessageQ;
OS_TMR ShieldRechargeTimer;

static OS_TMR ShieldActiveTimer;
static OS_TCB shieldTCB;
static CPU_STK shieldSTK[STACK_SIZES];
static OS_MUTEX ShieldMutex;

struct ShieldState shieldState;

void shieldTask(void);
void shieldActiveCB(void);
void shieldRechargeCB(void);


void shieldTaskCreate (void) {
    RTOS_ERR taskErr;
    RTOS_ERR qErr;
    RTOS_ERR tmrErr1;
    RTOS_ERR tmrErr2;
    RTOS_ERR mutErr;

    OSTaskCreate(
                 &shieldTCB,                        /* Pointer to the task's TCB.  */
                 "Shield Task",                     /* Name to help debugging.     */
                 (OS_TASK_PTR)&shieldTask,                       /* Pointer to the task's code. */
                 DEF_NULL,                          /* Pointer to task's argument. */
                 NORMAL_PRIO,                       /* Task's priority.            */
                 &shieldSTK[0],                     /* Pointer to base of stack.   */
                 (STACK_SIZES / 10u),               /* Stack limit, from base.     */
                 STACK_SIZES,                       /* Stack size, in CPU_STK.     */
                 10u,                               /* Messages in task queue.     */
                 120u,                              /* Round-Robin time quanta.    */
                 DEF_NULL,                          /* External TCB data.          */
                 OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &taskErr);

    OSQCreate(
        &ShieldMessageQ,
        "Shield Message Queue",
        4,
        &qErr);

    OSTmrCreate(
        &ShieldActiveTimer,
        "Shield Active Timer",
        SHIELD_ACTIVE_100MS,
        0,
        OS_OPT_TMR_ONE_SHOT,
        (OS_TMR_CALLBACK_PTR)&shieldActiveCB,
        NULL,
        &tmrErr1);

    OSTmrCreate(
        &ShieldRechargeTimer,
        "Shield Recharge Timer",
        SHIELD_RECHARGE_100MS,
        0,
        OS_OPT_TMR_ONE_SHOT,
        (OS_TMR_CALLBACK_PTR)&shieldRechargeCB,
        NULL,
        &tmrErr2);

    OSMutexCreate(
        &ShieldMutex,
        "Shield Mutex",
        &mutErr);

    if(taskErr.Code || qErr.Code || tmrErr1.Code || tmrErr2.Code || mutErr.Code) EFM_ASSERT(false);

    shieldState.active = false;
    shieldState.recharging = false;
}

void shieldTask(void)
{
  RTOS_ERR qErr;
  RTOS_ERR tmrErr;
  RTOS_ERR mutErr;

  uint8_t *msg;
  OS_MSG_SIZE size = 1;
  //OS_MSG_SIZE *size = malloc(sizeof(OS_MSG_SIZE));

  while(1)
  {
      msg = OSQPend(&ShieldMessageQ, 0, OS_OPT_PEND_BLOCKING, &size, NULL, &qErr);
      if(qErr.Code) EFM_ASSERT(false);

      OSMutexPend(&ShieldMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      if(*msg == 1 && !shieldState.recharging)
      {
          shieldState.active = true;
          OSTmrStart(&ShieldActiveTimer, &tmrErr);
          if(tmrErr.Code) EFM_ASSERT(false);
      } else if(*msg == 0 && shieldState.active)
      {
          shieldState.active = false;
          shieldState.recharging = true;
          OSTmrStart(&ShieldRechargeTimer, &tmrErr);
          if(tmrErr.Code) EFM_ASSERT(false);
      }

      OSMutexPost(&ShieldMutex, OS_OPT_POST_NONE, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      free(msg);
      //free(size);
  }
}

void shieldActiveCB(void)
{
  RTOS_ERR tmrErr;
  RTOS_ERR mutErr;

  OSMutexPend(&ShieldMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
  if(mutErr.Code) EFM_ASSERT(false);

  if(shieldState.active)
  {
      shieldState.active = false;
      shieldState.recharging = true;
      OSTmrStart(&ShieldRechargeTimer, &tmrErr);
      if(tmrErr.Code) EFM_ASSERT(false);
  }

  OSMutexPost(&ShieldMutex, OS_OPT_POST_NONE, &mutErr);
  if(mutErr.Code) EFM_ASSERT(false);
}

void shieldRechargeCB(void)
{
  RTOS_ERR mutErr;

  OSMutexPend(&ShieldMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
  if(mutErr.Code) EFM_ASSERT(false);

  if(shieldState.recharging) shieldState.recharging = false;

  OSMutexPost(&ShieldMutex, OS_OPT_POST_NONE, &mutErr);
  if(mutErr.Code) EFM_ASSERT(false);
}

void shieldRechargeTimer(void)
{
  RTOS_ERR mutErr;

  OSMutexPend(&ShieldMutex, 0, OS_OPT_POST_NONE, NULL, &mutErr);
  if(mutErr.Code) EFM_ASSERT(false);

  if(shieldState.recharging) shieldState.recharging = false;

  OSMutexPost(&ShieldMutex, OS_OPT_POST_NONE, &mutErr);
  if(mutErr.Code) EFM_ASSERT(false);
}
