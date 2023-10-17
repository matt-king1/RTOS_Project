/*
 * gameManager.h
 *
 *  Created on: Dec 15, 2022
 *      Author: matt
 */

#ifndef GAMEMANAGER_GAMEMANAGER_H_
#define GAMEMANAGER_GAMEMANAGER_H_

enum gameState_e {PREGAME = 0x1, INPROGRESS = 0x2, GAMEOVER = 0x4};

void gameOver(char cause[]);
void decreaseLife(void);
void startGame(void);
void gameStoppedTask(void);
void gameStoppedTaskCreate(void);

#endif /* GAMEMANAGER_GAMEMANAGER_H_ */
