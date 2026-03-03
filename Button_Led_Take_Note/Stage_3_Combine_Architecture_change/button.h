#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f10x.h"

// Các "Lời chẩn đoán" mà hệ thống nút bấm có thể phát ra
typedef enum {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_SINGLE_CLICK,
    BTN_EVENT_DOUBLE_CLICK,
    BTN_EVENT_HOLD
} ButtonEvent_TypeDef;

// "Hồ sơ bệnh án" của một nút bấm
typedef struct {
    unsigned char       pin_state;        // 0: Bị đè, 1: Đang nhả
    unsigned int        press_duration;   // Đo thời gian đè
    unsigned int        release_duration; // Đo thời gian chờ nhịp 2
    unsigned char       click_count;      // Số lần click
    ButtonEvent_TypeDef event_code;       // Sự kiện chốt lại
} Button_Context;

// Khai báo 2 nút để main.c có thể soi hồ sơ
extern Button_Context btn_PA0;
extern Button_Context btn_PA1;

// Khai báo hàm
void button_state_hardware_scan(void);
void button_fsm_process(Button_Context *btn);

#endif // BUTTON_H