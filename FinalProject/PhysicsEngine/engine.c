/*
 * engine.c
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */
#include "engine.h"
#include <math.h>

extern uint8_t HM_COUNT;
extern struct HoltzmanData HMs[];
extern OS_MUTEX HMMutex;
extern OS_MUTEX PlatformMutex;
extern struct PlatformData platformData;
extern struct ShieldState shieldState;
extern int PLATFORM_BOUNCE_ENABLED;
extern int MAX_SPEED;
extern int score;
extern uint8_t autoCannon;

OS_TMR PhysicsTimer;

static OS_TCB engineTCB;
static CPU_STK engineSTK[STACK_SIZES];
static OS_SEM PhysicsSem;

int GRAVITY_PIXELS = 25;

void engineTask(void);

void physEngineTimerCB(void)
{
  RTOS_ERR semErr;
  OSSemPost(&PhysicsSem, OS_OPT_POST_1, &semErr);
  if(semErr.Code) EFM_ASSERT(false);
}

void engineTaskCreate (void) {
    RTOS_ERR     semErr, tmrErr, tskErr;

    OSSemCreate(&PhysicsSem,
                "Physics Engine Semaphore",
                0,
                &semErr);

    OSTmrCreate(&PhysicsTimer,
                "Physics Engine Timer",
                0,
                PHYSICS_PERIOD,
                OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR)&physEngineTimerCB,
                NULL,
                &tmrErr);

    OSTaskCreate(&engineTCB,                /* Pointer to the task's TCB.  */
                 "Physics Engine Task",                    /* Name to help debugging.     */
                 (OS_TASK_PTR)&engineTask,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  HIGHER_PRIO,             /* Task's priority.            */
                 &engineSTK[0],             /* Pointer to base of stack.   */
                 (STACK_SIZES / 10u),  /* Stack limit, from base.     */
                  STACK_SIZES,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  120u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &tskErr);

    if(semErr.Code || tmrErr.Code || tskErr.Code) EFM_ASSERT(false);
}

// update HMs via incremental physics calculations
void updateHMs(struct HoltzmanData HMs_[])
{
  for(int i = 0; i < HM_COUNT; i++)
  {
      HMs_[i].x += HMs_[i].vx * PHYSICS_DELTA;
      HMs_[i].y += HMs_[i].vy * PHYSICS_DELTA;
      HMs_[i].vy += GRAVITY_PIXELS * PHYSICS_DELTA;
      if((HMs_[i].x - HM_PIXEL_RADIUS) < CANYON_START) HMs_[i].vx = fabs(HMs_[i].vx);
      else if((HMs_[i].x + HM_PIXEL_RADIUS) > CANYON_END) HMs_[i].vx = -1 * fabs(HMs_[i].vx);
  }
}

// update platform via incremental physics calculations and check for wall and HM collisions
void updatePlatform(struct PlatformData *platData_)
{
  platData_->x += platData_->vx * PHYSICS_DELTA;
  platData_->vx += platData_->ax * PHYSICS_DELTA;

  if((platData_->x - (PLATFORM_WIDTH/2)) < CANYON_START) // left wall collision
  {
      if(platData_->vx < (-1 * MAX_SPEED)) gameOver("Too Fast!");
      else if(PLATFORM_BOUNCE_ENABLED)
      {
          platData_->ax = 0;
          platData_->vx = fabs(platData_->vx);
      } else
      {
          platData_->ax = 0;
          platData_->vx = 0;
          platData_->x = CANYON_START + PLATFORM_WIDTH/2;
      }
  } else if((platData_->x + (PLATFORM_WIDTH/2)) > CANYON_END) // right wall collision
  {
      if(platData_->vx > MAX_SPEED) gameOver("Too Fast!");
      else if(PLATFORM_BOUNCE_ENABLED)
      {
          platData_->ax = 0;
          platData_->vx = -1 * fabs(platData_->vx);
          platData_->x = CANYON_END - PLATFORM_WIDTH/2;
      } else
      {
          platData_->ax = 0;
          platData_->vx = 0;
          platData_->x = CANYON_END - PLATFORM_WIDTH/2;
      }
  }
}

// check if HMs collide with platform or escape the top of the screen
void checkHMs(struct HoltzmanData HMs_[], struct PlatformData *platData_, struct ShieldState *shieldState_)
{
  RTOS_ERR mutErr;

  for(int i = 0; i < HM_COUNT; i++)
  {
      if((HMs_[i].y + HM_PIXEL_RADIUS) >= PLATFORM_Y && HMs_[i].vy > 0) // HM at platform
      {
          if(HMs_[i].vy > GAME_OVER_SPEED &&
             HMs_[i].x < (platData_->x + (PLATFORM_WIDTH/2)) &&
             HMs_[i].x > (platData_->x - (PLATFORM_WIDTH/2))) // HM hits the platform
          {
              HMs_[i].vy *= shieldState_->active ? ACTIVE_KINETIC_GAIN : PASSIVE_KINETIC_REDUCTION; // reflect HM at speed depending on if shield is active
          } else
          {
              if(!autoCannon || !(shootLaser(i))) // if no lasers left
              {
                  decreaseLife();

                  OSMutexPend(&HMMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
                  if(mutErr.Code) EFM_ASSERT(false);

                  generateHM(i);

                  OSMutexPost(&HMMutex, OS_OPT_POST_NONE, &mutErr);
                  if(mutErr.Code) EFM_ASSERT(false);
              }
          }
      } else if (HMs_[i].y < 0) // HM reached the top
      {
          OSMutexPend(&HMMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
          if(mutErr.Code) EFM_ASSERT(false);

          generateHM(i);

          OSMutexPost(&HMMutex, OS_OPT_POST_NONE, &mutErr);
          if(mutErr.Code) EFM_ASSERT(false);

          score++;
      }
  }
}

// physics engine task to update HMs, update platform, and check HMs for collision or escape
void engineTask(void)
{
  RTOS_ERR semErr, mutErr;

  while(1)
  {
      OSSemPend(&PhysicsSem, 0, OS_OPT_PEND_BLOCKING, NULL, &semErr);
      if(semErr.Code) EFM_ASSERT(false);

      OSMutexPend(&HMMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      updateHMs(HMs);

      OSMutexPost(&HMMutex, OS_OPT_POST_NONE, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      OSMutexPend(&PlatformMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      updatePlatform(&platformData);

      OSMutexPost(&PlatformMutex, OS_OPT_POST_NONE, &mutErr);
      if(mutErr.Code) EFM_ASSERT(false);

      checkHMs(HMs, &platformData, &shieldState);
  }
}
