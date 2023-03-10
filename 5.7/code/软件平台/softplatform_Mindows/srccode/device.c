
#include "device.h"


/***********************************************************************************
函数功能: 初始化软件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void software_init(void)
{
    /* 初始化缓冲池 */
    buf_pool_init();

    /* 通信环形缓冲初始化 */
    msg_rx_ring_buf_init();
    msg_tx_ring_buf_init();
}

/***********************************************************************************
函数功能: 初始化硬件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void hardware_init(void)
{
    /* 初始化串口1 */
    uart1_init();

    /* 初始化串口2 */
    uart2_init();
}

