#ifndef STM32F103C8T6_H
#define STM32F103C8T6_H

#include <stdint.h>

// 1. Định nghĩa cấu trúc thanh ghi RCC dựa trên Datasheet (RM0008)
// Cấu trúc này phải xếp ĐÚNG THỨ TỰ vì mỗi thanh ghi cách nhau 4 bytes (32-bit)
typedef struct {
  volatile uint32_t CR;         // Offset: 0x00
  volatile uint32_t CFGR;       // Offset: 0x04
  volatile uint32_t CIR;        // Offset: 0x08
  volatile uint32_t APB2RSTR;   // Offset: 0x0C
  volatile uint32_t APB1RSTR;   // Offset: 0x10
  volatile uint32_t AHBENR;     // Offset: 0x14
  volatile uint32_t APB2ENR;    // Offset: 0x18 (ĐÂY LÀ THANH GHI LIE DÙNG!)
  volatile uint32_t APB1ENR;    // Offset: 0x1C
  volatile uint32_t BDCR;       // Offset: 0x20
  volatile uint32_t CSR;        // Offset: 0x24
} RCC_TypeDef;

// 2. Định nghĩa địa chỉ gốc của RCC (Giống file main.c của Lie)
#define RCC_BASE_ADDR   0x40021000

// 3. Ép kiểu địa chỉ đó thành con trỏ tới Struct RCC_TypeDef
#define RCC             ((RCC_TypeDef *) RCC_BASE_ADDR)

// 4. Định nghĩa vị trí Bit của TIM1 trong thanh ghi APB2ENR (Bit số 11)
#define RCC_APB2ENR_TIM1EN    (1UL << 11)

#endif