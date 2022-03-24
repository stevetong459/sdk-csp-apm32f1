/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2022-03-21     Stevetong459  first version
 */

#include <string.h>
#include <stdlib.h>
#include <apm32f10x.h>
#include <board.h>
#include <apm32f10x_usart.h>
#include <apm32f10x_gpio.h>
#include <apm32f10x_rcm.h>

struct apm32_usart
{
    USART_T *usart;
    const char *tx_pin_name;
    const char *rx_pin_name;
};

struct apm32_usart usart_config[] =
{
#ifdef BSP_USING_UART1
    {
        USART1,
        BSP_UART1_TX_PIN,
        BSP_UART1_RX_PIN
    }
#endif
#ifdef BSP_USING_UART2
    {
        USART2,
        BSP_UART2_TX_PIN,
        BSP_UART2_RX_PIN
    }
#endif
};

unsigned char _uart_port_get(const char *str)
{
    if (str[0] == 'P')
    {
        return str[1];
    }
    else
    {
        return 0;
    }
}

unsigned char _uart_pin_get(const char *str)
{
    /* return the value of pin, such as PA10 will return 10 */
    if (str[0] == 'P')
    {
        if (str[3] == '\0')
        {
            return str[2] - '0';
        }
        else
        {
            return (str[2] - '0') * 10 + str[3] - '0';
        }
    }
    else
    {
        return 0;
    }
}

int uart_init()
{
    GPIO_Config_T GPIO_ConfigStruct;
    USART_Config_T USART_ConfigStruct;
    GPIO_T *gpio_port;
    uint8_t i;

    for (i = 0; i < sizeof(usart_config) / sizeof(struct apm32_usart); i++)
    {
        gpio_port = (GPIO_T *)(GPIOA_BASE + 0x400 * (_uart_port_get(usart_config[i].rx_pin_name) - 'A'));
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA << (_uart_port_get(usart_config[i].rx_pin_name) - 'A'));

        GPIO_ConfigStruct.mode = GPIO_MODE_AF_PP;
        GPIO_ConfigStruct.pin = 1 << _uart_pin_get(usart_config[i].rx_pin_name);
        GPIO_ConfigStruct.pin |= 1 << _uart_pin_get(usart_config[i].tx_pin_name);
        GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
        GPIO_Config(gpio_port, &GPIO_ConfigStruct);

        USART_ConfigStruct.baudRate = 115200;
        USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
        USART_ConfigStruct.mode = USART_MODE_TX;
        USART_ConfigStruct.parity = USART_PARITY_NONE;
        USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
        USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;

#ifdef BSP_USING_UART1
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);
        USART_Config(USART1, &USART_ConfigStruct);
        USART_Enable(USART1);
#endif
#ifdef BSP_USING_UART2
        RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART2);
        USART_Config(USART2, &USART_ConfigStruct);
        USART_Enable(USART2);
#endif
    }

    return 0;
}

void print_char(char c)
{
#if defined(BSP_USING_UART1)
    while(USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
    USART_TxData(USART1, (uint8_t)c);
#elif defined BSP_USING_UART2
    while(USART_ReadStatusFlag(USART2, USART_FLAG_TXBE) == RESET);
    USART_TxData(USART2, (uint8_t)c);
#endif
}
