#include "stm32f10x.h"
#include "button.h"
#include "led.h"

volatile unsigned int global_tick = 0; 
volatile unsigned char xx = 0x00; 

void SysTick_Handler(void) {
    global_tick++; 
}

int main(void) {
    unsigned char last_xx = 0xFF; // Biến cờ nhớ trạng thái cũ của xx

    // 1. Cấp xung nhịp (Port A cho Nút, Port B cho LED)
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN);

    // 2. Cấu hình Nút nhấn (PA0, PA1: Input Pull-up)
    GPIOA->CRL &= ~(0xFF << 0);            
    GPIOA->CRL |= (0x88 << 0);             
    GPIOA->ODR |= (1UL << 0) | (1UL << 1); 

    // 3. Cấu hình LED (PB12: Output Push-pull)
    GPIOB->CRH &= ~(0xF << 16); 
    GPIOB->CRH |= (0x3 << 16); 
    GPIOB->ODR |= (1UL << 12); // Tắt PB12 ban đầu

    // 4. Kích hoạt SysTick (1ms)
    SysTick_Config(SystemCoreClock / 1000); 

    while (1) {
        // ========================================================
        // GIAI ĐOẠN 1: THU THẬP TỪ PHẦN CỨNG NÚT BẤM
        // ========================================================
        button_state_hardware_scan();
        button_fsm_process(&btn_PA0, global_tick);
        button_fsm_process(&btn_PA1, global_tick);

        // ========================================================
        // GIAI ĐOẠN 2: CHUYỂN SỰ KIỆN NÚT THÀNH TRẠNG THÁI APP (xx)
        // ========================================================
				// XỬ LÝ NÚT PA0
				{
					if (btn_PA0.event_code == BTN_EVENT_SINGLE_CLICK) {
							xx = (xx == 0x01) ? 0x02 : 0x01; // Lật 0x01 và 0x02
							btn_PA0.event_code = BTN_EVENT_NONE; 
					}
					else if (btn_PA0.event_code == BTN_EVENT_HOLD) {
							xx = 0x04;
							btn_PA0.event_code = BTN_EVENT_NONE; 
					}
					// THÊM ĐOẠN NÀY: Bắt Double Click của PA0 để Reset trạng thái vi PA0 khong co DOUBLE CLICK
					else if (btn_PA0.event_code == BTN_EVENT_DOUBLE_CLICK) {
							xx = 0x00; 
							btn_PA0.event_code = BTN_EVENT_NONE; 
					}
					else if (btn_PA0.event_code != BTN_EVENT_NONE) {
							btn_PA0.event_code = BTN_EVENT_NONE; // Dọn rác
					}
					}
        // XỬ LÝ NÚT PA1
				{
					if (btn_PA1.event_code == BTN_EVENT_DOUBLE_CLICK) {
							xx = 0x03;
							btn_PA1.event_code = BTN_EVENT_NONE; 
					}
					else if (btn_PA1.event_code != BTN_EVENT_NONE) {
							btn_PA1.event_code = BTN_EVENT_NONE; // Dọn rác
					}
				}
        // ========================================================
        // GIAI ĐOẠN 3: ĐIỀU PHỐI LED (CHỈ GỌI KHI xx THAY ĐỔI)
        // ========================================================
        if (xx != last_xx) {
            last_xx = xx; // Cập nhật ngay để không bị gọi lại ở vòng lặp sau
            
            if (xx == 0x01 || xx == 0x02) {
                led_set_mode(&led_PB12, LED_MODE_FADE_IN, global_tick);
            }
            else if (xx == 0x04) {
                led_set_mode(&led_PB12, LED_MODE_FADE_OUT, global_tick);
            }
            else if (xx == 0x03) {
                led_set_mode(&led_PB12, LED_MODE_ASYNC_BLINK, global_tick);
            }
            else if (xx == 0x00) { // Khi xx bị ép về 0 (ví dụ bấm Reset)
                led_set_mode(&led_PB12, LED_MODE_OFF, global_tick);
            }
        }

        // ========================================================
        // GIAI ĐOẠN 4: THỰC THI HIỆU ỨNG LED
        // ========================================================
        led_process(&led_PB12, global_tick);
    }
}