/*
 * STM32F407 Bare-Metal UART Interrupt Example
 * USART2 | TX: PA2 | RX: PA3
 * Baud: 9600 (HSI 16 MHz default)
 *
 * Features:
 * - RX interrupt
 * - TX interrupt
 * - Echo typed characters
 * - ENTER -> new line
 * - BACKSPACE -> delete last character
 */

#include <stdint.h>

/* =========================================================
   BASE ADDRESSES
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

/* CR1 bits */
#define USART_CR1_UE       (1U << 13)
#define USART_CR1_TE       (1U << 3)
#define USART_CR1_RE       (1U << 2)
#define USART_CR1_RXNEIE   (1U << 5)
#define USART_CR1_TXEIE    (1U << 7)

/* SR bits */
#define USART_SR_RXNE      (1U << 5)
#define USART_SR_TXE       (1U << 7)

/* =========================================================
   NVIC
   ========================================================= */
#define NVIC_ISER1         (*(volatile uint32_t *)0xE000E104UL)
#define USART2_IRQn       38

/* =========================================================
   ASCII CODES
   ========================================================= */
#define ASCII_ENTER        '\r'
#define ASCII_BACKSPACE   0x08
#define ASCII_DELETE      0x7F

/* =========================================================
   BUFFERS
   ========================================================= */
#define RX_BUF_SIZE 64

volatile char rx_buffer[RX_BUF_SIZE];
volatile uint8_t rx_index = 0;

volatile char tx_buffer[3];
volatile uint8_t tx_len = 0;
volatile uint8_t tx_pos = 0;

/* =========================================================
   UART INIT
   ========================================================= */
void uart2_init(void)
{
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

    /* PA2, PA3 AF mode */
    GPIOA_MODER &= ~(3U << (2 * 2));
    GPIOA_MODER |=  (2U << (2 * 2));

    GPIOA_MODER &= ~(3U << (2 * 3));
    GPIOA_MODER |=  (2U << (2 * 3));

    /* AF7 for USART2 */
    GPIOA_AFRL &= ~(0xFU << (4 * 2));
    GPIOA_AFRL |=  (7U << (4 * 2));

    GPIOA_AFRL &= ~(0xFU << (4 * 3));
    GPIOA_AFRL |=  (7U << (4 * 3));

    /* 9600 baud @ 16 MHz */
    USART2_BRR = 0x068B;

    USART2_CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART2_CR1 |= USART_CR1_RXNEIE;
    USART2_CR1 |= USART_CR1_UE;

    NVIC_ISER1 |= (1U << (USART2_IRQn - 32));
}

/* =========================================================
   USART2 INTERRUPT HANDLER
   ========================================================= */
void USART2_IRQHandler(void)
{
    /* RX interrupt */
    if (USART2_SR & USART_SR_RXNE)
    {
        char c = (char)USART2_DR;

        if (c == ASCII_ENTER)
        {
            rx_buffer[rx_index] = '\0';
            rx_index = 0;

            tx_buffer[0] = '\r';
            tx_buffer[1] = '\n';
            tx_len = 2;
            tx_pos = 0;

            USART2_CR1 |= USART_CR1_TXEIE;
        }
        else if ((c == ASCII_BACKSPACE || c == ASCII_DELETE) && rx_index > 0)
        {
            rx_index--;

            tx_buffer[0] = '\b';
            tx_buffer[1] = ' ';
            tx_buffer[2] = '\b';
            tx_len = 3;
            tx_pos = 0;

            USART2_CR1 |= USART_CR1_TXEIE;
        }
        else if (rx_index < RX_BUF_SIZE - 1)
        {
            rx_buffer[rx_index++] = c;

            tx_buffer[0] = c;
            tx_len = 1;
            tx_pos = 0;

            USART2_CR1 |= USART_CR1_TXEIE;
        }
    }

    /* TX interrupt */
    if ((USART2_SR & USART_SR_TXE) && (tx_pos < tx_len))
    {
        USART2_DR = tx_buffer[tx_pos++];
        if (tx_pos >= tx_len)
        {
            USART2_CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

/* =========================================================
   MAIN
   ========================================================= */
int main(void)
{
    uart2_init();

    while (1)
    {
        __asm__("nop");   // CPU idle
    }
}

