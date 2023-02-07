
#include "device.h"


/***********************************************************************************
函数功能: 初始化软件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_SoftwareInit(void)
{
    ;
}

/***********************************************************************************
函数功能: 初始化硬件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_HardwareInit(void)
{
    /* 初始化串口 */
    DEV_UartInit();
}

/***********************************************************************************
函数功能: 初始化串口.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_UartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable USART1 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1
                           | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - Hardware flow control disabled (RTS and CTS signals)
    */
    USART_StructInit(&USART_InitStructure);

    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}

/***********************************************************************************
函数功能: 向串口发送一个字符.
入口参数: ucChar: 输出的字符.
返 回 值: none.
***********************************************************************************/
void DEV_PutChar(U8 ucChar)
{
    /* Loop until USART1 DR register is empty */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        ;
    }

    USART_SendData(USART1, ucChar);
}

/***********************************************************************************
函数功能: 向串口发送字符串, 采用软件查询方式.
入口参数: pucChar: 输出的字符串指针.
返 回 值: none.
***********************************************************************************/
void DEV_PutString(U8* pucChar)
{
    U8 i;

    i = 0;

    while(0 != *(pucChar + i))
    {
        DEV_PutChar(*(pucChar + i));
        i++;
    }
}

/***********************************************************************************
函数功能: 初始化系统tick定时器.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_SysTickInit(void)
{
    (void)SysTick_Config(SystemCoreClock / (1000 / TICK));
}

