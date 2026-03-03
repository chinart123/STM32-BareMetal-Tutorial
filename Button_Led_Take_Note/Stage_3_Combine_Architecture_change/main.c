#include "stm32f10x.h"
#include "button.h"

// Biến điều khiển tổng (Watch 1 soi ở đây)
volatile unsigned char xx = 0x00; 

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 2000; j++);  
}

int main(void) {
    unsigned char toggle_pa0 = 0;  // Nhớ trạng thái sáng/tắt của PC13
    unsigned int  blink_timer = 0; // Bộ đếm nháy PB12

    // 1. Cấp Clock
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);

    // 2. Cấu hình PA0 và PA1 (Input Pull-up)
    GPIOA->CRL &= ~(0xFF << 0);           // Xóa cấu hình của Pin 0 và Pin 1
    GPIOA->CRL |= (0x88 << 0);            // Đặt thành Input Pull-up
    GPIOA->ODR |= (1UL << 0) | (1UL << 1); // Kéo cả PA0 và PA1 lên mức 1

    // 3. Cấu hình PC13 và PB12 (Output Push-pull, mặc định tắt)
    GPIOC->CRH &= ~(0xF << 20); GPIOC->CRH |= (0x3 << 20); GPIOC->ODR |= (1UL << 13);
    GPIOB->CRH &= ~(0xF << 16); GPIOB->CRH |= (0x3 << 16); GPIOB->ODR |= (1UL << 12);

    while (1) {
        // =====================================================
        // GIAI ĐOẠN 1 + 2: THU THẬP VÀ XỬ LÝ
        // =====================================================
        button_state_hardware_scan();
        
        button_fsm_process(&btn_PA0); // Khám bệnh cho PA0
        button_fsm_process(&btn_PA1); // Khám bệnh cho PA1

        // =====================================================
        // GIAI ĐOẠN 3: THỰC THI (ĐỌC HỒ SƠ VÀ RA LỆNH)
        // =====================================================
        
        // --- XỬ LÝ HỒ SƠ CỦA PA0 ---
        if (btn_PA0.event_code == BTN_EVENT_SINGLE_CLICK) {
            toggle_pa0 = !toggle_pa0;
            if (toggle_pa0) {
                xx = 0x01;
                GPIOC->BRR = (1UL << 13); // Sáng PC13
            } else {
                xx = 0x02;
                GPIOC->BSRR = (1UL << 13); // Tắt PC13
            }
            btn_PA0.event_code = BTN_EVENT_NONE; // Dọn hồ sơ
        }
        else if (btn_PA0.event_code == BTN_EVENT_HOLD) {
            xx = 0x04;
            btn_PA0.event_code = BTN_EVENT_NONE; // Dọn hồ sơ
        }

        // --- XỬ LÝ HỒ SƠ CỦA PA1 ---
        if (btn_PA1.event_code == BTN_EVENT_DOUBLE_CLICK) {
            xx = 0x03;
            GPIOC->ODR ^= (1UL << 13); // Đảo PC13 báo hiệu
            btn_PA1.event_code = BTN_EVENT_NONE; // Dọn hồ sơ
        }

        // --- HÀM DUY TRÌ LIÊN TỤC CHO 0x04 ---
        if (xx == 0x04) {
            blink_timer++;
            if (blink_timer >= 50) {
                GPIOB->ODR ^= (1UL << 12); // Chớp PB12
                blink_timer = 0;
            }
        } else {
            GPIOB->BSRR = (1UL << 12); // Luôn tắt PB12 nếu xx != 4
            blink_timer = 0;
        }

        // Trễ nhịp hệ thống (10ms)
        delay_ms(10); 
    }
}