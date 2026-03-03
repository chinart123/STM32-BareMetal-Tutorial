#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f10x.h"

// Biến toàn cục từ main.c
extern volatile unsigned char xx; 

// Khai báo các hàm điều khiển
void button_process(void);
unsigned char button_check(unsigned char target_xx);
void button_press(void);
void button_hold_toggle(void);

#endif // BUTTON_H