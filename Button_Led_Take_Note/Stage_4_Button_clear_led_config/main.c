#include "stm32f10x.h"
#include "button.h"

volatile unsigned int global_tick = 0; 
volatile unsigned char xx = 0x00; 

void SysTick_Handler(void) {
    global_tick++; 
}

int main(void) {
    // 1. Cấp xung nhịp
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // 2. Cấu hình CẢ PA0 và PA1 (Input Pull-up)
    GPIOA->CRL &= ~(0xFF << 0);            // Xóa rác Pin 0, Pin 1
    GPIOA->CRL |= (0x88 << 0);             // Cấu hình Input Pull-up
    GPIOA->ODR |= (1UL << 0) | (1UL << 1); // Kéo điện trở lên mức 1

    // 3. Kích hoạt SysTick (1ms)
    SysTick_Config(SystemCoreClock / 1000); 

    while (1) {
        
        // --- KHỐI 1: THU THẬP & PHÂN TÍCH FSM ---
        button_state_hardware_scan();
        
        // Quăng sổ của 2 nút vào máy FSM, truyền kèm thời gian SysTick
        button_fsm_process(&btn_PA0, global_tick);
        button_fsm_process(&btn_PA1, global_tick);

        // --- KHỐI 2: ĐIỀU PHỐI (App Logic) ---
        
        // XỬ LÝ NÚT PA0 (Single Click -> 0x01/0x02, Hold -> 0x04)
        if (btn_PA0.event_code == BTN_EVENT_SINGLE_CLICK) {
            if (xx == 0x01) xx = 0x02;
            else xx = 0x01;
            btn_PA0.event_code = BTN_EVENT_NONE; 
        }
        else if (btn_PA0.event_code == BTN_EVENT_HOLD) {
            xx = 0x04;
            btn_PA0.event_code = BTN_EVENT_NONE; 
        }
        // Ép dọn rác nếu người dùng cố tình nhấn đúp PA0 (vì PA0 không hỗ trợ đúp)
        else if (btn_PA0.event_code != BTN_EVENT_NONE) {
            btn_PA0.event_code = BTN_EVENT_NONE; 
        }

        // XỬ LÝ NÚT PA1 (Double Click -> 0x03)
        if (btn_PA1.event_code == BTN_EVENT_DOUBLE_CLICK) {
            xx = 0x03;
            btn_PA1.event_code = BTN_EVENT_NONE; 
        }
        // Ép dọn rác nếu người dùng nhấn đơn hoặc đè PA1 (vì PA1 chỉ rình nhấn đúp)
        else if (btn_PA1.event_code != BTN_EVENT_NONE) {
            btn_PA1.event_code = BTN_EVENT_NONE; 
        }
    }
}