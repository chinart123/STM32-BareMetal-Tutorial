#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f10x.h"

// Các sự kiện chuẩn
typedef enum {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_SINGLE_CLICK,
    BTN_EVENT_DOUBLE_CLICK,
    BTN_EVENT_HOLD
} ButtonEvent_TypeDef;

// Sổ khám bệnh (Thêm biến last_update_tick để tự căn giờ)
typedef struct {
    unsigned char       pin_state;        
    unsigned int        press_duration;   
    unsigned int        release_duration; 
    unsigned char       click_count;      
    ButtonEvent_TypeDef event_code;       
    unsigned int        last_update_tick; // MỐC THỜI GIAN ĐỂ CHẠY FSM MỖI 10ms
} Button_Context;

extern Button_Context btn_PA0;
extern Button_Context btn_PA1; // Khai báo thêm bệnh nhân PA1
void button_state_hardware_scan(void);
void button_fsm_process(Button_Context *btn, unsigned int current_tick);

#endif