/*
 * Bare-metal UART (TX + RX) Echo Example
 * Target  : STM32F407
 * UART    : USART2
 * Pins    : PA2 (TX), PA3 (RX)
 * Baud    : 9600 (HSI 16 MHz default clock)
 */

#include <stdint.h>

/* =========================================================
   Peripheral base addresses
   ========================================================= */
#define PERIPH_BASE        0x40000000UL
#define AHB1PERIPH_BASE    0x40020000UL
#define APB1PERIPH_BASE    0x40000000UL

/* =========================================================
   RCC
   ========================================================= */
#define RCC_BASE           0x40023800UL
#define RCC_AHB1ENR        (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB1ENR        (*(volatile uint32_t *)(RCC_BASE + 0x40))

#define RCC_AHB1ENR_GPIOAEN   (1U << 0)
#define RCC_APB1ENR_USART2EN  (1U << 17)

/* =========================================================
   GPIOA
   ========================================================= */
#define GPIOA_BASE         0x40020000UL
#define GPIOA_MODER        (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_AFRL         (*(volatile uint32_t *)(GPIOA_BASE + 0x20))

/* =========================================================
   USART2
   ========================================================= */
#define USART2_BASE        0x40004400UL
#define USART2_SR          (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART2_DR          (*(volatile uint32_t *)(USART2_BASE + 0x04))
#define USART2_BRR         (*(volatile uint32_t *)(USART2_BASE + 0x08))
#define USART2_CR1         (*(volatile uint32_t *)(USART2_BASE + 0x0C))

/* USART control bits */
#define USART_CR1_UE       (1U << 13)
#define USART_CR1_TE       (1U << 3)
#define USART_CR1_RE       (1U << 2)

/* USART status bits */
#define USART_SR_TXE       (1U << 7)
#define USART_SR_RXNE      (1U << 5)

/* =========================================================
   Delay (crude, CPU blocking)
   ========================================================= */
static void delay(void)
{
    for (volatile uint32_t i = 0; i < 50000; i++);
}

/* =========================================================
   USART2 Initialization
   ========================================================= */
static void uart2_init(void)
{
    /* Enable clocks */
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

    /* PA2, PA3 -> Alternate Function mode */
    GPIOA_MODER &= ~(3U << (2 * 2));
    GPIOA_MODER |=  (2U << (2 * 2));   // PA2 AF

    GPIOA_MODER &= ~(3U << (2 * 3));
    GPIOA_MODER |=  (2U << (2 * 3));   // PA3 AF

    /* AF7 (USART2) for PA2, PA3 */
    GPIOA_AFRL &= ~(0xFU << (4 * 2));
    GPIOA_AFRL |=  (7U  << (4 * 2));

    GPIOA_AFRL &= ~(0xFU << (4 * 3));
    GPIOA_AFRL |=  (7U  << (4 * 3));

    /* Baud rate: 9600 @ 16 MHz (HSI default) */
    USART2_BRR = 0x068B;

    /* Enable TX and RX */
    USART2_CR1 |= USART_CR1_TE | USART_CR1_RE;

    /* Enable USART */
    USART2_CR1 |= USART_CR1_UE;
}

/* =========================================================
   UART transmit / receive
   ========================================================= */
static void uart2_send_char(char c)
{
    while (!(USART2_SR & USART_SR_TXE));
    USART2_DR = (uint32_t)c;
}

static char uart2_recv_char(void)
{
    while (!(USART2_SR & USART_SR_RXNE));
    return (char)USART2_DR;
}

static void uart2_send_string(const char *str)
{
    while (*str)
    {
        uart2_send_char(*str++);
    }
}

/* =========================================================
   Main: UART Echo
   ========================================================= */
int main(void)
{
    char ch;

    uart2_init();
    uart2_send_string("UART Echo Ready\r\n");

    while (1)
    {
        ch = uart2_recv_char();   // wait for input
        uart2_send_char(ch);      // echo back

        if (ch == '\r')
        {
            uart2_send_char('\n');
        }
    }
}

