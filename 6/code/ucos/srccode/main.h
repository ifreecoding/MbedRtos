
#include <string.h>
#include "stm32f10x.h"
#include "ucos_ii.h"


/* 定义变量类型 */
typedef unsigned char           U8;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef signed char             S8;
typedef short                   S16;
typedef int                     S32;

#undef NULL
#define NULL                    ((void*)0)

/* 返回值 */
#define RTN_SUCD                0       /* 成功 */
#define RTN_FAIL                1       /* 失败 */


#define QUE_RING_LEN            10      /* 队列所使用的环形缓冲长度 */
#define TASK_STASK_LEN          100     /* 任务所使用的栈长度, 单位: 4字节 */

#define BUF_LEN                 1000    /* 消息缓冲长度, 单位: 字节 */


/* 消息缓冲结构 */
typedef struct msg_buf
{
    U32 len;            /* 数据长度 */
    U8 buf[BUF_LEN];    /* 缓冲 */
}MSG_BUF;


extern U32 socket_tcp_connect(U8* pserver_ip, U32 server_port, U32 client_port);
extern void rx_thread(void* pdata);
extern void handle_thread(void* pdata);
extern void tx_thread(void* pdata);
extern void msg_rx(void);
extern void msg_process(void);
extern void msg_tx(void);
extern void sys_tick_init(void);
extern void uart1_init(void);
extern void start_to_send_data(void);
extern void msg_rx_isr(void);
extern void msg_tx_isr(void);

