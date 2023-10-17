/*
 * lcd.h
 *
 *  Created on: Nov 15, 2022
 *      Author: matt
 */

#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include "os.h"
#include "constants.h"

#define CANNON_LENGTH 17

void LCD_init(void);
void lcdTask(void);
void lcdTimerCB(void);

#endif /* LCD_LCD_H_ */

