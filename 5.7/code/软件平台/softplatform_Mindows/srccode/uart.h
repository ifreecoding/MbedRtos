
#ifndef  __UART_H__
#define  __UART_H__

#include "global.h"


#define RING_BUF_LEN                512     /* 环形接收缓冲长度, 单位: 字节 */


#define UART_TX_IDLE                 0      /* 串口硬件发送空闲态, 可以发送数据 */
#define UART_TX_BUSY                 1      /* 串口硬件发送忙态, 无法发送数据 */


/* 环形缓冲, 头指针指向第一个有效字符, 尾指针指向第一个无效字符, 头尾指针相等时表示
   缓冲为空 */
typedef struct ring_buf
{
    U32 head;                       /* 环形缓冲头索引 */
    U32 tail;                       /* 环形缓冲尾索引 */
    U8 buf[RING_BUF_LEN];           /* 环形缓冲 */
}RING_BUF;


extern void copy_data_from_ring_rx_buf(U8* pbuf, U32 len);
extern void copy_data_to_ring_tx_buf(U8* pbuf, U32 len);


#endif

