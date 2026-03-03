#ifndef LED_H
#define LED_H

#include "stm32f10x.h"

// 4 chế độ hoạt động chuẩn theo yêu cầu
typedef enum {
    LED_MODE_OFF = 0,
    LED_MODE_FADE_IN,     // xx = 0x01 hoặc 0x02
    LED_MODE_ASYNC_BLINK, // xx = 0x03
    LED_MODE_FADE_OUT     // xx = 0x04
} LedMode_TypeDef;

// "Hồ sơ" của đèn LED
typedef struct {
    LedMode_TypeDef mode;
    unsigned int    brightness;     // Mức sáng hiện tại: 0 đến 20
    unsigned int    last_slow_tick; // Mốc thời gian để Fading hoặc Blink
    unsigned char   is_on_phase;    // Đang ở pha sáng (1) hay pha tắt (0) - Dùng cho Blink
} Led_Context;

extern Led_Context led_PB12;

void led_set_mode(Led_Context *led, LedMode_TypeDef new_mode, unsigned int current_tick);
void led_process(Led_Context *led, unsigned int current_tick);

#endif // LED_H