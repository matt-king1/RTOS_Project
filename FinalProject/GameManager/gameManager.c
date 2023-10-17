/*
 * game_management.c
 *
 *  Created on: Dec 2, 2022
 *      Author: matt
 */

#include "gameManager.h"
#include "os.h"
#include "constants.h"
#include "HM/hm.h"
#include "Platform/platform.h"
#include "string.h"
#include "stdlib.h"

OS_FLAG_GRP GameStateFlag;
OS_Q ButtonQ;
enum gameState_e gameState = PREGAME;
int cursor_pos = 0;
char causeOfDeath[32] = "";
int score = 0;
int highScore = 0;
int lives = 3;


static OS_TCB gameTCB;
static CPU_STK gameSTK[STACK_SIZES];

extern OS_TMR LEDTimers[];
extern OS_TMR PhysicsTimer;
extern OS_TMR PlatformTimer;
extern int PLATFORM_BOUNCE_ENABLED;
extern struct PlatformData platformData;
extern uint8_t laserCharges;
extern uint8_t HM_COUNT;
extern bool autoCannon;


void gameOver(char cause[]) {
  RTOS_ERR flgErr;
  RTOS_ERR tmrErr;
  gameState = GAMEOVER;
  cursor_pos = 0;
  if (score > highScore) highScore = score;
  strcpy(causeOfDeath, cause);
  for (int i = 0; i < 3; i++) {
      if(OSTmrStateGet(&LEDTimers[i], &tmrErr) == OS_TMR_STATE_RUNNING) {
          OSTmrStop(&LEDTimers[i], OS_OPT_TMR_NONE, NULL, &tmrErr);
          if (tmrErr.Code) EFM_ASSERT(false);
      }
  }
  turnOffLED();
  OSTmrStop(&PhysicsTimer, OS_OPT_TMR_NONE, NULL, &tmrErr);
  if (tmrErr.Code) EFM_ASSERT(false);
  OSTmrStop(&PlatformTimer, OS_OPT_TMR_NONE, NULL, &tmrErr);
  if (tmrErr.Code) EFM_ASSERT(false);
  OSFlagPost(&GameStateFlag, GAMEOVER, OS_OPT_POST_FLAG_SET, &flgErr);
  if (flgErr.Code) EFM_ASSERT(false);
  OSFlagPost(&GameStateFlag, INPROGRESS, OS_OPT_POST_FLAG_CLR, &flgErr);
  if (flgErr.Code) EFM_ASSERT(false);
}

void decreaseLife(void) {
  lives--;
  if (lives == 0) {
      gameOver("HM Hit");
  }
}

void startGame() {
  RTOS_ERR tmrErr;
  RTOS_ERR flgErr;

  platformData.ax = 0;
  platformData.vx = 0;
  platformData.x = SCREEN_PIXELS / 2;
  laserCharges = 5;
  lives = 3;
  score = 0;
  hm_init();
  gameState = INPROGRESS;

  OSTmrStart(&LEDTimers[0], &tmrErr);
  if (tmrErr.Code != RTOS_ERR_NONE) EFM_ASSERT(false);
  OSTmrStart(&PhysicsTimer, &tmrErr);
  OSFlagPost(&GameStateFlag, INPROGRESS, OS_OPT_POST_FLAG_SET, &flgErr);
  if (tmrErr.Code != RTOS_ERR_NONE || flgErr.Code) EFM_ASSERT(false);
  OSTmrStart(&PlatformTimer, &tmrErr);
  OSFlagPost(&GameStateFlag, PREGAME | GAMEOVER, OS_OPT_POST_FLAG_CLR, &flgErr);
  if (flgErr.Code || tmrErr.Code) EFM_ASSERT(false);
}

void gameStoppedTaskCreate(void) {
  RTOS_ERR tskErr;
  RTOS_ERR flgErr;
  RTOS_ERR qErr;
  OSTaskCreate(
     &gameTCB,                /* Pointer to the task's TCB.  */
    "game Task.",                    /* Name to help debugging.     */
    (OS_TASK_PTR)&gameStoppedTask,                   /* Pointer to the task's code. */
     DEF_NULL,                          /* Pointer to task's argument. */
     HIGHER_PRIO,             /* Task's priority.            */
    &gameSTK[0],             /* Pointer to base of stack.   */
    (STACK_SIZES / 10u),  /* Stack limit, from base.     */
     STACK_SIZES,         /* Stack size, in CPU_STK.     */
     10u,                               /* Messages in task queue.     */
     120u,                                /* Round-Robin time quanta.    */
     DEF_NULL,                          /* External TCB data.          */
     OS_OPT_TASK_STK_CHK,               /* Task options.               */
    &tskErr
  );
  OSFlagCreate(
      &GameStateFlag,
      "Game State Flags",
      PREGAME,
      &flgErr
  );
  OSQCreate(
      &ButtonQ,
      "Button Queue",
      4,
      &qErr
  );
  if (flgErr.Code || tskErr.Code || qErr.Code) EFM_ASSERT(false);
}

void gameStoppedTask(void) {
  RTOS_ERR flgErr, qErr;

  uint8_t * btnMsg;
//  OS_MSG_SIZE *size = malloc(sizeof(OS_MSG_SIZE));
  OS_MSG_SIZE size = 1;
  while (1) {
      while (OSFlagPend(&GameStateFlag, PREGAME | GAMEOVER, 0, OS_OPT_PEND_FLAG_SET_ANY, NULL, &flgErr) != INPROGRESS) {
          btnMsg = OSQPend(&ButtonQ, 0, OS_OPT_PEND_BLOCKING, &size, NULL,&qErr);
          if (qErr.Code) EFM_ASSERT(false);
          if(*btnMsg == 1) {
              cursor_pos = (cursor_pos + 1) % 4;
          } else {
              switch (cursor_pos) {
                case 0:
                  HM_COUNT = ((HM_COUNT + 1) % 4 < 1) ? 1 : ((HM_COUNT + 1) % 4);
                  break;
                case 1:
                  PLATFORM_BOUNCE_ENABLED = !PLATFORM_BOUNCE_ENABLED;
                  break;
                case 2:
                  autoCannon = !autoCannon;
                  break;
                case 3:
                  startGame();
                  break;
                default:
                  EFM_ASSERT(false);
                  break;
              }
          }
          free(btnMsg);
          //free(size);
      }
  }
}
