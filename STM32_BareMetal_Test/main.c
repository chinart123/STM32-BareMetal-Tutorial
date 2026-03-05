
//Test Hardfault() của flash khi chạy ở 128MHz(HSE x PLL16)
//Vào trong hàm SetSysClockTo72() trong file "system_stm32f10x.c"
//Chỉnh RCC_CFGR_PLLMULL9 thành RCC_CFGR_PLLMULL16
//Chỉnh SYSCLK_FREQ_72MHz thành 128000000 
// Hàm delay cùi bắp (vòng lặp vô nghĩa) để test chớp tắt nếu chip vô tình còn sống
#include "stm32f10x.h"

// ==========================================================
// HÀM DELAY XỊN XÒ DÙNG SYSTICK (Mặc định lõi đang là 128MHz)
// ==========================================================
//void Delay_ms(uint32_t ms) {
//    // Nếu chạy 128MHz: 1ms = 128.000 nhịp
//    // (Nếu chạy 72MHz thì chỗ này là 72000 - 1)
//    SysTick->LOAD = 128000 - 1; 
//    SysTick->VAL = 0;
//    SysTick->CTRL = 5; // Bật đồng hồ, dùng xung lõi
//    
//    for(uint32_t i = 0; i < ms; i++) {
//        // CPU đứng chờ đồng hồ cát chảy hết 1ms
//        while((SysTick->CTRL & (1 << 16)) == 0); 
//    }
//    SysTick->CTRL = 0; // Tắt đồng hồ
//}
// Hàm Delay "Nạp thẳng" - Đo đếm giới hạn sức chịu đựng của SysTick
void Delay_Bao_Luc(uint32_t ms) {
    // Ép SysTick nạp thẳng tổng số nhịp cần thiết
    SysTick->LOAD = (ms * 128000) - 1; 
    SysTick->VAL = 0;
    SysTick->CTRL = 5;
    
    // CPU đứng chờ
    while((SysTick->CTRL & (1 << 16)) == 0); 
    SysTick->CTRL = 0;
}
int main(void) {
    // 1. Cấp điện cho Port B và Port C
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

    // 2. Cấu hình PC13 (Output Push-Pull, max 50MHz)
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x3 << 20);

    // 3. Cấu hình PB0 và PB1 (Output Push-Pull, max 50MHz)
    GPIOB->CRL &= ~(0xFF << 0);
    GPIOB->CRL |= (0x33 << 0); 

    // Bật LED lần đầu
    GPIOC->BSRR = (1 << 29); // Bật PC13 (kéo xuống 0)
    GPIOB->BSRR = (1 << 0) | (1 << 1); // Bật PB0, PB1 (kéo lên 1)

    while(1) {
        // Đảo trạng thái 3 đèn LED
        GPIOC->ODR ^= (1 << 13);
        GPIOB->ODR ^= (1 << 0) | (1 << 1);
        
        //Delay_ms(1000);// Gọi trễ đúng 1000 mili-giây (1 giây) bằng SysTick
        
				//Delay_Bao_Luc(131); //còn nháy được
			  Delay_Bao_Luc(20); //nháy siêu nhanh
				//Delay_Bao_Luc(132); //hết nháy luôn, nháy ông nội nhanh
    }
}

//Bây giờ, hãy thay hàm delay trong main bằng 2 kịch bản này và quan sát:Kịch bản 1 - Ranh giới an toàn: Chú gọi Delay_Bao_Luc(131);Toán học: $131 \times 128,000 = 16,768,000$.Số này nhỏ hơn sức chứa 16,777,215 của thanh ghi 24-bit.Kết quả: LED chớp nháy bình thường ở chu kỳ khoảng 0.13 giây.Kịch bản 2 - Vượt quá giới hạn (BUNG KHỎI): Chú gọi Delay_Bao_Luc(132); (Chỉ tăng đúng 1ms).Toán học: $132 \times 128,000 = 16,896,000$. (Mã Hex là 0x101E000).Thanh ghi 24-bit sẽ chặt đứt phần đầu, chỉ lấy khúc đuôi là 0x01E000 (tương đương 122,880 nhịp).122.880 nhịp ở 128MHz thực chất chỉ là... 0.96 mili-giây!Kết quả: Đèn LED đột ngột hóa điên, chớp tắt liên tục nhanh như máy khâu (vì Delay từ 132ms bị sụp xuống chỉ còn chưa tới 1ms).
