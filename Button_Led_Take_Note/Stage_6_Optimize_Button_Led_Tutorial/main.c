#include "stm32f10x.h"
#include "button.h"
#include "led.h"

volatile unsigned int global_tick = 0; 
volatile unsigned char xx = 0x00; 

void SysTick_Handler(void) {
    global_tick++; 
}

int main(void) {
    unsigned char last_xx = 0xFF; 

    // 1. Khởi tạo xung nhịp và GPIO
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);

    GPIOA->CRL &= ~(0xFF << 0);            
    GPIOA->CRL |= (0x88 << 0);             
    GPIOA->ODR |= (1UL << 0) | (1UL << 1); 

    GPIOB->CRH &= ~(0xF << 16); GPIOB->CRH |= (0x3 << 16); GPIOB->ODR |= (1UL << 12); 
    GPIOC->CRH &= ~(0xF << 20); GPIOC->CRH |= (0x3 << 20); GPIOC->ODR |= (1UL << 13); 

    // 2. Khởi động SysTick
    SysTick_Config(SystemCoreClock / 1000); 

    while (1) {
        // ==================================================
        // KHỐI 1: INPUT LAYER (Nút bấm)
        // ==================================================
        button_state_hardware_scan();
        button_fsm_process(&btn_PA0, global_tick);
        button_fsm_process(&btn_PA1, global_tick);

        // ==================================================
        // KHỐI 2: APPLICATION LAYER (Gán xx)
        // ==================================================
        
        // --- CỦA NÚT PA0 ---
        if (btn_PA0.event_code == BTN_EVENT_SINGLE_CLICK) {
            xx = (xx == 0x01) ? 0x02 : 0x01;
            btn_PA0.event_code = BTN_EVENT_NONE; 
        }
        else if (btn_PA0.event_code == BTN_EVENT_HOLD) { // Giữ 5s
            xx = 0x04;
            btn_PA0.event_code = BTN_EVENT_NONE; 
        }
        else if (btn_PA0.event_code == BTN_EVENT_DOUBLE_CLICK) {
            xx = 0x00; // Reset
            btn_PA0.event_code = BTN_EVENT_NONE; 
        }
        else if (btn_PA0.event_code != BTN_EVENT_NONE) btn_PA0.event_code = BTN_EVENT_NONE; 

        // --- CỦA NÚT PA1 ---
        if (btn_PA1.event_code == BTN_EVENT_DOUBLE_CLICK) {
            xx = 0x03;
            btn_PA1.event_code = BTN_EVENT_NONE; 
        }
        else if (btn_PA1.event_code == BTN_EVENT_HOLD) { // Giữ 2s
            xx = 0x05;
            btn_PA1.event_code = BTN_EVENT_NONE; 
        }
        else if (btn_PA1.event_code != BTN_EVENT_NONE) btn_PA1.event_code = BTN_EVENT_NONE; 

        // ==================================================
        // KHỐI 3: MIDDLEWARE LAYER (Ra lệnh LED dựa theo xx)
        // ==================================================
        if (xx != last_xx) {
            last_xx = xx; 
            
            // Xóa sạch trạng thái cũ trước khi cấp lệnh mới để không bị loạn
            led_set_mode(&led_PB12, LED_MODE_OFF, 0, 0, global_tick);
            led_set_mode(&led_PC13, LED_MODE_OFF, 0, 0, global_tick);

            // CẤP LỆNH MỚI:
            if (xx == 0x01 || xx == 0x02) {
                // PB12 Sáng dần
                led_set_mode(&led_PB12, LED_MODE_FADE_IN, 0, 0, global_tick);
            }
            else if (xx == 0x04) {
                // PB12 Tối dần
                led_set_mode(&led_PB12, LED_MODE_FADE_OUT, 0, 0, global_tick);
            }
            else if (xx == 0x03) {
                // PB12 Sáng 1s (1000), tắt 0.5s (500)
                led_set_mode(&led_PB12, LED_MODE_ASYNC_BLINK, 1000, 500, global_tick);
            }
            else if (xx == 0x05) {
                // Yêu cầu: PC13 tắt 1s, sáng 0.5s -> Sáng 500, Tắt 1000
                led_set_mode(&led_PC13, LED_MODE_ASYNC_BLINK, 500, 1000, global_tick);
								led_set_mode(&led_PB12, LED_MODE_ASYNC_BLINK, 1000, 500, global_tick);
            }
            // Nếu xx == 0x00 thì code sẽ không làm gì thêm vì đã gọi tắt cả 2 LED ở trên rồi.
        }

        // ==================================================
        // KHỐI 4: OUTPUT LAYER (Phần cứng xuất tín hiệu)
        // ==================================================
        led_process(&led_PB12, global_tick);
        led_process(&led_PC13, global_tick);
        
        led_hardware_update(global_tick); 
    }
}