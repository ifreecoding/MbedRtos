
#ifndef  MDS_USERROOT_H
#define  MDS_USERROOT_H


#include <string.h>
#include "mindows.h"


#define TASK_STASK_LEN          400         /* 任务所使用的栈长度, 单位: 字节 */

#define BUF_LEN                 100         /* 消息缓冲长度, 单位: 字节 */

#define TEST_PROCESS_NUM        3           /* 测试进程数 */


#define MSG_FLAG_NONE           0           /* 没有消息 */
#define MSG_FLAG_NEED_SEND      1           /* 有消息需要发送 */


#define TEST_PROCESS_ADDR       0x08010000  /* 测试进程存放的起始地址 */
#define RESET_VECTOR_ADDR       0x04        /* 复位向量地址 */
#define TEST_PROCESS_MAX_SIZE   0x2000      /* 测试进程所占最大空间, 单位: 字节 */


/* 消息缓冲结构 */
typedef struct msg_buf
{
    U32 flag;           /* 缓冲标志 */
    U8 buf[BUF_LEN];    /* 缓冲 */
}MSG_BUF;


extern MSG_BUF gtx_buf[3];


extern void rx_thread(void* pdata);
extern void tx_thread(void* pdata);
extern void uart1_init(void);
extern void put_char(U8 send_char);
extern void put_string(U8* pstring);


#endif

