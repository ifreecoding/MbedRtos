
#include <string.h>
#include "stm32f10x.h"
#include "ucos_ii.h"


/* ����������� */
typedef unsigned char           U8;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef signed char             S8;
typedef short                   S16;
typedef int                     S32;

#undef NULL
#define NULL                    ((void*)0)

/* ����ֵ */
#define RTN_SUCD                0       /* �ɹ� */
#define RTN_FAIL                1       /* ʧ�� */


#define QUE_RING_LEN            10      /* ������ʹ�õĻ��λ��峤�� */
#define TASK_STASK_LEN          100     /* ������ʹ�õ�ջ����, ��λ: 4�ֽ� */

#define BUF_LEN                 1000    /* ��Ϣ���峤��, ��λ: �ֽ� */


/* ��Ϣ����ṹ */
typedef struct msg_buf
{
    U32 len;            /* ���ݳ��� */
    U8 buf[BUF_LEN];    /* ���� */
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

