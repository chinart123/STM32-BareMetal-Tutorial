#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f10x.h"

extern volatile unsigned char xx; 

// Các hàm đã được "cất đi" theo yêu cầu
// void button_process(void);
// void button_press(void);
// void button_hold_toggle(void);

// Các hàm cho bài toán Double Click
void button_process_double(void); // Hàm quét riêng cho Double Click
unsigned char button_check(unsigned char target_xx);
void button_double_click(void);

#endif // BUTTON_H